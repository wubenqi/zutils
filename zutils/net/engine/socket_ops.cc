// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/engine/socket_ops.h"

#include "base/logging.h"
#include "base/strings/stringprintf.h"

#if defined(OS_WIN)
#include <ws2tcpip.h>
#include "base/win/win_util.h"
#define EADDRNOTAVAIL WSAEADDRNOTAVAIL
#define EINPROGRESS WSAEINPROGRESS
#define EWOULDBLOCK WSAEWOULDBLOCK
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>
#include <setjmp.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include<linux/tcp.h>
#endif

namespace net {

#if defined(OS_WIN)
namespace win_util {

std::string FormatMessageA(unsigned messageid) {
	char* string_buffer = NULL;
	unsigned string_length = ::FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL, messageid, 0,
		reinterpret_cast<char *>(&string_buffer), 0, NULL);

	std::string formatted_string;
	if (string_buffer) {
    if (string_length>1) {
      formatted_string.assign(string_buffer, string_length-1);
    } else {
      formatted_string = string_buffer;
    } 
    
		LocalFree(reinterpret_cast<HLOCAL>(string_buffer));
	} else {
		// The formating failed. simply convert the message value into a string.
		base::SStringPrintf(&formatted_string, "message number %d", messageid);
	}
	return formatted_string;
}

}

std::string FormatLastWSAErrorA(unsigned messageid) {
	return win_util::FormatMessageA(messageid);
}

std::string FormatLastWSAErrorA() {
	return win_util::FormatMessageA(WSAGetLastError());
}
#endif

// used to ensure we delete the addrinfo structure
// alloc'd by getaddrinfo
class ScopedAddrinfo {
protected:
	struct addrinfo * addrinfo_ptr_;
public:

	explicit ScopedAddrinfo(struct addrinfo* addrinfo_ptr) :
		addrinfo_ptr_(addrinfo_ptr) {}

	~ScopedAddrinfo() {
		freeaddrinfo(addrinfo_ptr_);
	}
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Sets an FD to be nonblocking.
void SetNonBlocking(SOCKET s) {
#if defined(OS_WIN)
	unsigned long no_block = 1;
	int ioctl_return = ioctlsocket(s, FIONBIO, &no_block);
	DCHECK_GE(ioctl_return, 0)
		<< "error doing ioctlsocket(s, FIONBIO, &no_block) fd: " << s
		<< " errno=" << WSAGetLastError();

#elif defined(OS_POSIX)
	DCHECK_GE(s, 0);
	int fcntl_return = fcntl(s, F_GETFL, 0);
	CHECK_NE(fcntl_return, -1)
		<< "error doing fcntl(fd, F_GETFL, 0) fd: " << s
		<< " errno=" << errno;

	if (fcntl_return & O_NONBLOCK)
		return;

	fcntl_return = fcntl(s, F_SETFL, fcntl_return | O_NONBLOCK);
	CHECK_NE(fcntl_return, -1)
		<< "error doing fcntl(fd, F_SETFL, fcntl_return) fd: " << s
		<< " errno=" << errno;
#endif
}

int SetDisableNagle(SOCKET s) {
	int on = 1;
	int rc;
	rc = setsockopt(s, IPPROTO_TCP,  TCP_NODELAY,
		reinterpret_cast<char*>(&on), sizeof(on));
	if (rc < 0) {
#if defined(OS_WIN)
		closesocket(s);
#elif defined(OS_POSIX)
		close(s);
#endif
		LOG(FATAL) << "setsockopt() TCP_NODELAY: failed on socket " << s;
		return 0;
	}
	return 1;
}

// Summary:
//   Closes a socket, with option to attempt it multiple times.
//   Why do this? Well, if the system-call gets interrupted, close
//   can fail with EINTR. In that case you should just retry.. Unfortunately,
//   we can't be sure that errno is properly set since we're using a
//   multithreaded approach in the filter proxy, so we should just retry.
// Args:
//   fd - the socket to close
//   tries - the number of tries to close the socket.
// Returns:
//   true - if socket was closed
//   false - if socket was NOT closed.
// Side-effects:
//   sets *fd to -1 if socket was closed.
//
bool CloseSocket(SOCKET *s, int tries) {
	for (int i = 0; i < tries; ++i) {
#if defined(OS_WIN)
		if (!closesocket(*s)) {
			*s = kInvalidSocket;
			return true;
		}
#elif defined(OS_POSIX)
		if (!close(*s)) {
			*s = kInvalidSocket;
			return true;
		}
#endif
	}
	return false;
}

int CreateTCPListeningSocket(const std::string& ip, const std::string& port, bool is_numeric_host_address, SOCKET* listen_socket) {
	// start out by assuming things will fail.
	*listen_socket = kInvalidSocket;

	const char* node = NULL;
	const char* service = NULL;

	if (!ip.empty()) node = ip.c_str();
	if (!port.empty()) service = port.c_str();

	struct addrinfo *results = 0;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));

	if (is_numeric_host_address) {
		hints.ai_flags = AI_NUMERICHOST;  // iff you know the name is numeric.
	}
	hints.ai_flags |= AI_PASSIVE;
	
	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int err = 0;
	if ((err=getaddrinfo(node, service, &hints, &results))) {
		// gai_strerror -is- threadsafe, so we get to use it here.
		LOG(ERROR) << "getaddrinfo " << " for (" << ip << ":" << port
					<< ") " << gai_strerror(err) << "\n";
		return -1;
	}
	// this will delete the addrinfo memory when we return from this function.
	ScopedAddrinfo addrinfo_guard(results);

	SOCKET sock = socket(results->ai_family,
						results->ai_socktype,
						results->ai_protocol);
	if (sock == kInvalidSocket) {

#if defined(OS_WIN)
		LOG(ERROR) << "Unable to create socket for (" << ip << ":"
			<< port << "): " << win_util::FormatMessageA(WSAGetLastError()) << "\n";
#else
		LOG(ERROR) << "Unable to create socket for (" << ip << ":"
			<< port << "): " << strerror(errno) << "\n";
#endif
		return -1;
	}

	if (true) {
		// set SO_REUSEADDR on the listening socket.
		int on = 1;
		int rc;
		rc = setsockopt(sock, SOL_SOCKET,  SO_REUSEADDR,
			reinterpret_cast<char *>(&on), sizeof(on));
		if (rc < 0) {
#if defined(OS_WIN)
			closesocket(sock);
#elif defined(OS_POSIX)
			close(sock);
#endif
			LOG(FATAL) << "setsockopt() SO_REUSEADDR failed socket=" << sock << "\n";
		}
	}

#ifndef SO_REUSEPORT
#define SO_REUSEPORT 15
#endif
	if (false) {
		// set SO_REUSEADDR on the listening socket.
		int on = 1;
		int rc;
		rc = setsockopt(sock, SOL_SOCKET,  SO_REUSEPORT,
			reinterpret_cast<char *>(&on), sizeof(on));
		if (rc < 0) {
#if defined(OS_WIN)
			closesocket(sock);
#elif defined(OS_POSIX)
			close(sock);
#endif
			LOG(FATAL) << "setsockopt() SO_REUSEPORT failed socket=" << sock << "\n";
		}
	}

	int result = bind(sock, results->ai_addr, results->ai_addrlen);
	if (result < 0) {
		// If we are waiting for the interface to be raised, such as in an
		// HA environment, ignore reporting any errors.

#if defined(OS_WIN)
		int saved_errno = WSAGetLastError();
		if ( saved_errno != EADDRNOTAVAIL) {
			LOG(ERROR) << "Bind was unsuccessful for (" << ip << ":"
				<< port << "): " << win_util::FormatMessageA(saved_errno) << "\n";
		}
#else
		int saved_errno = errno;
		if ( errno != EADDRNOTAVAIL) {
			LOG(ERROR) << "Bind was unsuccessful for (" << ip << ":"
				<< port << "): " << strerror(errno) << "\n";
		}
#endif
		// if we knew that we were not multithreaded, we could do the following:
		// " : " << strerror(errno) << "\n";
		if (CloseSocket(&sock, 100)) {
			if ( saved_errno == EADDRNOTAVAIL ) {
				return -3;
			}
			return -2;
		} else {
			// couldn't even close the dang socket?!
			LOG(ERROR) << "Unable to close the socket.. Considering this a fatal "
				"error, and exiting\n";
			exit(EXIT_FAILURE);
			return -1;
		}
	}

	if (!SetDisableNagle(sock)) {
		return -1;
	}

	int backlog = 1024;
	result = listen(sock, backlog);
	if (result < 0) {
		// listen was unsuccessful.

#if defined(OS_WIN)
		LOG(ERROR) << "Listen was unsuccessful for (" << ip << ":"
			<< port << "): " << FormatLastWSAErrorA() << "\n";
		// if we knew that we were not multithreaded, we could do the following:
		// " : " << strerror(errno) << "\n";
#else
		LOG(ERROR) << "Listen was unsuccessful for (" << ip << ":"
			<< port << "): " << strerror(errno) << "\n";
		// if we knew that we were not multithreaded, we could do the following:
		// " : " << strerror(errno) << "\n";
#endif

		if (CloseSocket(&sock, 100)) {
			sock = kInvalidSocket;
			return -1;
		} else {
			// couldn't even close the dang socket?!
			LOG(FATAL) << "Unable to close the socket.. Considering this a fatal "
				"error, and exiting\n";
		}
	}

	// If we've gotten to here, Yeay! Success!
	*listen_socket = sock;

	return 0;
}

int CreateTCPConnectedSocket(const std::string& ip, const std::string& port, bool is_numeric_host_address, SOCKET *connect_socket) {
	const char* node = NULL;
	const char* service = NULL;

	*connect_socket = kInvalidSocket;
	if (!ip.empty())
		node = ip.c_str();
	if (!port.empty())
		service = port.c_str();

	struct addrinfo *results = 0;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));

	if (is_numeric_host_address) {
		hints.ai_flags = AI_NUMERICHOST;  // iff you know the name is numeric.
	}
	//hints.ai_flags |= AI_PASSIVE;

	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int err = 0;
	if ((err=getaddrinfo(node, service, &hints, &results))) {
		// gai_strerror -is- threadsafe, so we get to use it here.
		LOG(ERROR) << "getaddrinfo for (" << ip << ":" << port << "): "
			<< gai_strerror(err);
		return -1;
	}
	// this will delete the addrinfo memory when we return from this function.
	ScopedAddrinfo addrinfo_guard(results);


	//int sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	int sock = socket(results->ai_family,
		results->ai_socktype,
		results->ai_protocol);
	if (sock == kInvalidSocket) {
#if defined(OS_WIN)
		LOG(ERROR) << "Unable to create socket for (" << ip << ":" << port
			<< "): " << FormatLastWSAErrorA();
#else
		LOG(ERROR) << "Unable to create socket for (" << ip << ":" << port
			<< "): " << strerror( errno );
#endif
		return -1;
	}

	SetNonBlocking( sock );
	if (!SetDisableNagle(sock)) {
		return -1;
	}

	int ret_val = 0;

	if ( connect( sock, results->ai_addr, results->ai_addrlen )==-1 ) {
#if defined(OS_WIN)
		int err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK && err != WSAEINVAL && err != WSAEINPROGRESS) {
			LOG(ERROR) << "Connect was unsuccessful for (" << ip << ":" << port
				<< "): " << FormatLastWSAErrorA();
			closesocket( sock );
			return -1;
		}
#else
		// if ( errno != EAGAIN && errno != EWOULDBLOCK ) {
    if ( errno != EINPROGRESS ) {
			LOG(ERROR) << "Connect was unsucessful for (" << ip << ":" << port
				<< "): " << strerror(errno);
			close( sock );
			return -1;
		}
#endif
	} else {
		ret_val = 1;
	}

/*
	// 测试用
	struct timeval timeout ;
	fd_set r;
	FD_ZERO(&r);
	FD_SET(sock, &r);

	timeout.tv_sec = 15; //连接超时15秒
	timeout.tv_usec =0;
	int ret = select(0, 0, &r, 0, &timeout);
	if ( ret <= 0 ) {
		::closesocket(sock);
		return -1;
	} 
	ret_val = 1;
 */

	// SetNonBlocking( sock );

	// If we've gotten to here, Yeay! Success!
	*connect_socket = sock;

	return ret_val;
}

#define kDefaultConnTimeout	30

// 考虑的还不是太完善
// 需要仔细再考虑
bool CreateBlockingTCPConnectedSocket(const std::string& ip, const std::string& port, bool is_numeric_host_address, SOCKET *connect_socket) {
	*connect_socket = kInvalidSocket;

	SOCKET sock = kInvalidSocket;
	int ret_val = CreateTCPConnectedSocket(ip, port, is_numeric_host_address, &sock);
	if (ret_val==0) { 
		// 等待连接
		int rv;

		struct timeval tv ;
		fd_set rd_wr, ex;

		int os_error = 0;
		socklen_t len = sizeof(os_error);

		tv.tv_sec = kDefaultConnTimeout; //连接超时kDefaultConnTimeout秒
		tv.tv_usec =0;

		FD_ZERO(&rd_wr);
		FD_ZERO(&ex);

		FD_SET(sock, &rd_wr);
		FD_SET(sock, &ex);

#if defined(OS_WIN)
		rv = select(0, NULL, &rd_wr, &ex, &tv);
#else
		rv = select(0, NULL, &rd_wr, NULL, &tv);
#endif

  if (rv > 0) {
#if defined(OS_WIN)
    /*
    * Call Sleep(0) to work around a Winsock timing bug.
    */
    Sleep(0);
    if (FD_ISSET(sock, &ex)) {
      if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *) &os_error, &len) == SOCKET_ERROR) {  
        LOG(ERROR) << "Connect error " << FormatLastWSAErrorA();
        os_error = WSAGetLastError();
        return false;
      }
      if (os_error != 0) {
        LOG(ERROR) << "Connect error " << FormatLastWSAErrorA(os_error);
      } else {
        LOG(ERROR) << "Connect unknow error";
      }
      return false;
    }
    if (FD_ISSET(sock, &rd_wr)) {
      /* it's connected */
      *connect_socket = sock;
      return true;
    }
#else
    if (FD_ISSET(sock, &rd_wr)) {

      // Get the error that connect() completed with.
      int os_error = 0;

      if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&os_error, &len) < 0) {
        os_error = errno;
        LOG(ERROR) << "Connect host failed: " << strerror( os_error );
        return false;
      }

      if (os_error!=0) {
        LOG(ERROR) << "Connect host failed: " << strerror( os_error );
        return false;
      }

      /* it's connected */
      *connect_socket = sock;
      return true;
    }

#endif
  } if (rv==0) {
#if defined(OS_WIN)
    LOG(ERROR) << "select error " << FormatLastWSAErrorA(WSAETIMEDOUT);
    closesocket( sock );
    return false;
#else
    LOG(ERROR) << "select error " << strerror( ETIMEDOUT );
    close( sock );
#endif
    return false;
  } else {
#if defined(OS_WIN)
    LOG(ERROR) << "select error " << FormatLastWSAErrorA();
    closesocket( sock );
#else
    LOG(ERROR) << "select error " << strerror( errno );
    close( sock );
#endif
    return false;
  }

  return true;
 } else if (ret_val==-1) {
   return false;
 }
 return true;
}

int Accept(SOCKET acceptor, SOCKET *accepted_socket) {
  *accepted_socket = kInvalidSocket;
  sockaddr_in from;
  socklen_t from_len = sizeof(from);
  SOCKET conn = accept(acceptor, reinterpret_cast<sockaddr*>(&from), &from_len);
  if (conn == kInvalidSocket) {
#if defined(OS_WIN)
#else
    if (errno != 11) {
      LOG(ERROR) << "Acceptor: accept fail("
        << acceptor << "): " << errno << ": "
        << strerror(errno);
    }
#endif
    return -1;
  }

  SetNonBlocking(conn);
  if (!SetDisableNagle(conn)) {
    return -1;
  }
  *accepted_socket = conn;
  return 0;
}


}


/*

http://bdxnote.blog.163.com/blog/static/844423520098651256549/
非阻塞connect

C/VC/C++ 2009-09-06 17:12:56 阅读1511 评论0   字号：大中小 订阅
在一个TCP套接口被设置为非阻塞之后调用connect,connect会立即返回EINPROGRESS错误,表示连接操作正在进行中,但是仍未完成;同时TCP的三路握手操作继续进行;在这之后,我们可以调用select来检查这个链接是否建立成功;非阻塞connect有三种用途:
1.我们可以在三路握手的同时做一些其它的处理.connect操作要花一个往返时间完成,而且可以是在任何地方,从几个毫秒的局域网到几百毫秒或几秒的广域网.在这段时间内我们可能有一些其他的处理想要执行;
2.可以用这种技术同时建立多个连接.在Web浏览器中很普遍;
3.由于我们使用select来等待连接的完成,因此我们可以给select设置一个时间限制,从而缩短connect的超时时间.在大多数实现中,connect的超时时间在75秒到几分钟之间.有时候应用程序想要一个更短的超时时间,使用非阻塞connect就是一种方法;
非阻塞connect听起来虽然简单,但是仍然有一些细节问题要处理:
1.即使套接口是非阻塞的,如果连接的服务器在同一台主机上,那么在调用connect建立连接时,连接通常会立即建立成功.我们必须处理这种情况;
2.源自Berkeley的实现(和Posix.1g)有两条与select和非阻塞IO相关的规则:
A:当连接建立成功时,套接口描述符变成可写;
B:当连接出错时,套接口描述符变成既可读又可写;
注意:当一个套接口出错时,它会被select调用标记为既可读又可写;

非阻塞connect有这么多好处,但是处理非阻塞connect时会遇到很多可移植性问题;

处理非阻塞connect的步骤:
第一步:创建socket,返回套接口描述符;
第二步:调用fcntl把套接口描述符设置成非阻塞;
第三步:调用connect开始建立连接;
第四步:判断连接是否成功建立;
A:如果connect返回0,表示连接简称成功(服务器可客户端在同一台机器上时就有可能发生这种情况);
B:调用select来等待连接建立成功完成;
如果select返回0,则表示建立连接超时;我们返回超时错误给用户,同时关闭连接,以防止三路握手操作继续进行下去;
如果select返回大于0的值,则需要检查套接口描述符是否可读或可写;如果套接口描述符可读或可写,则我们可以通过调用getsockopt来得到套接口上待处理的错误(SO_ERROR),如果连接建立成功,这个错误值将是0,如果建立连接时遇到错误,则这个值是连接错误所对应的errno值(比如:ECONNREFUSED,ETIMEDOUT等).
"读取套接口上的错误"是遇到的第一个可移植性问题;如果出现问题,getsockopt源自Berkeley的实现是返回0,等待处理的错误在变量errno中返回;但是Solaris会让getsockopt返回-1,errno置为待处理的错误;我们对这两种情况都要处理;

这样,在处理非阻塞connect时,在不同的套接口实现的平台中存在的移植性问题,首先,有可能在调用select之前,连接就已经建立成功,而且对方的数据已经到来.在这种情况下,连接成功时套接口将既可读又可写.这和连接失败时是一样的.这个时候我们还得通过getsockopt来读取错误值;这是第二个可移植性问题;
移植性问题总结:
1.对于出错的套接口描述符,getsockopt的返回值源自Berkeley的实现是返回0,待处理的错误值存储在errno中;而源自Solaris的实现是返回0,待处理的错误存储在errno中;(套接口描述符出错时调用getsockopt的返回值不可移植)
2.有可能在调用select之前,连接就已经建立成功,而且对方的数据已经到来,在这种情况下,套接口描述符是既可读又可写;这与套接口描述符出错时是一样的;(怎样判断连接是否建立成功的条件不可移植)

这样的话,在我们判断连接是否建立成功的条件不唯一时,我们可以有以下的方法来解决这个问题:
1.调用getpeername代替getsockopt.如果调用getpeername失败,getpeername返回ENOTCONN,表示连接建立失败,我们必须以SO_ERROR调用getsockopt得到套接口描述符上的待处理错误;
2.调用read,读取长度为0字节的数据.如果read调用失败,则表示连接建立失败,而且read返回的errno指明了连接失败的原因.如果连接建立成功,read应该返回0;
3.再调用一次connect.它应该失败,如果错误errno是EISCONN,就表示套接口已经建立,而且第一次连接是成功的;否则,连接就是失败的;

被中断的connect:
如果在一个阻塞式套接口上调用connect,在TCP的三路握手操作完成之前被中断了,比如说,被捕获的信号中断,将会发生什么呢?假定connect不会自动重启,它将返回EINTR.那么,这个时候,我们就不能再调用connect等待连接建立完成了,如果再次调用connect来等待连接建立完成的话,connect将会返回错误值EADDRINUSE.在这种情况下,应该做的是调用select,就像在非阻塞式connect中所做的一样.然后,select在连接建立成功(使套接口描述符可写)或连接建立失败(使套接口描述符既可读又可写)时返回;
*/

