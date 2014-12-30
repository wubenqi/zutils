// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/engine/tcp_connector.h"

//#include "base/message_loop/message_loop.h"
#include "base/bind.h"

#include "net/base/net_errors.h"
//#include "net/engine/net_engine_manager.h"

#if defined(OS_WIN)
#include <ws2tcpip.h>
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

namespace {
#if defined(OS_WIN)
int MapConnectError(int os_error) {
switch (os_error) {
	// connect fails with WSAEACCES when Windows Firewall blocks the
	// connection.
	case WSAEACCES:
		return ERR_NETWORK_ACCESS_DENIED;
	case WSAETIMEDOUT:
		return ERR_CONNECTION_TIMED_OUT;
	default: {
		int net_error = MapSystemError(os_error);
		if (net_error == ERR_FAILED)
			return ERR_CONNECTION_FAILED;  // More specific than ERR_FAILED.

/*
		// Give a more specific error when the user is offline.
		if (net_error == ERR_ADDRESS_UNREACHABLE &&
			NetworkChangeNotifier::IsOffline()) {
				return ERR_INTERNET_DISCONNECTED;
		}
*/
		return net_error;
	 }
}
}
#else
int MapConnectError(int os_error) {
	switch (os_error) {
		case EACCES:
			return ERR_NETWORK_ACCESS_DENIED;
		case ETIMEDOUT:
			return ERR_CONNECTION_TIMED_OUT;
		default: {
			int net_error = MapSystemError(os_error);
			if (net_error == ERR_FAILED)
				return ERR_CONNECTION_FAILED;  // More specific than ERR_FAILED.

/*
			// Give a more specific error when the user is offline.
			if (net_error == ERR_ADDRESS_UNREACHABLE &&
				NetworkChangeNotifier::IsOffline()) {
					return ERR_INTERNET_DISCONNECTED;
			}
*/
			return net_error;
		 }
	}
}

#endif
}

TCPConnector::TCPConnector(base::MessageLoop* message_loop, TCPConnector::Delegate* delegate)	:
  connect_state_(kConnectStateInvalid),
  connector_(kInvalidSocket),
  message_loop_(message_loop),
  delegate_(delegate) {
	
	DCHECK(message_loop);
  DCHECK(message_loop->type()==base::MessageLoop::TYPE_CUSTOM);

  DCHECK(delegate);
}

void TCPConnector::ReConnect() {
  // 异步连接
  int ret = CreateTCPConnectedSocket(conn_addr_info_.ip, conn_addr_info_.port, conn_addr_info_.is_numeric_host_address, &connector_);
  if ( ret == -1 ) {
    LOG(ERROR) << "Unable to connect host for: ret = " << ret
      << ": " << conn_addr_info_.ip << ":"
      << conn_addr_info_.port;
    return;
  }

  if (ret==1) {
    OnNewConnection(connector_);
    // HandleConnectCompleted();
    return;
  }

  SetConnState(kConnectStateConnecting);
  OnConnecting();
}

bool TCPConnector::Connect(const std::string& ip, const std::string& port, bool is_numeric_host_address) {
  // 状态不合法
  if(GetConnectState()!=kConnectStateInvalid) {
    return false;
  }

  // 缓存ConnAddrInfo,为了断线重连
  conn_addr_info_.is_inited = true;
  conn_addr_info_.ip = ip;
  conn_addr_info_.port = port;
  conn_addr_info_.is_numeric_host_address = is_numeric_host_address;
  conn_addr_info_.is_reconnect = true;
  conn_addr_info_.reconnect_time = 10;

  DoConnect(0);
//   bool result = Connect(conn_addr_info_, false);
//   if (result == false) {
//     DoReconnect(reconnect_time);
//     // message_loop_->PostDelayedTask(FROM_HERE, base::Bind(&TCPConnector::ReConnect, base::Unretained(this), base::TimeDelta::FromSeconds(reconnect_time)));
//   }
  return true;
}

void TCPConnector::DoConnect(int reconnect_time) {
  if (reconnect_time == 0) {
    message_loop_->PostTask(FROM_HERE, base::Bind(&TCPConnector::ReConnect, base::Unretained(this)));
  } else {
    message_loop_->PostDelayedTask(FROM_HERE, base::Bind(&TCPConnector::ReConnect, base::Unretained(this)), base::TimeDelta::FromSeconds(reconnect_time));
  }
}

void TCPConnector::DoClose(bool from_me) {
  if (from_me) {
    if (!delegate_->OnDestroyConnection()) {
#if defined(OS_WIN)
      // Note: don't use CancelIo to cancel pending IO because it doesn't work
      // when there is a Winsock layered service provider.

      // In most socket implementations, closing a socket results in a graceful
      // connection shutdown, but in Winsock we have to call shutdown explicitly.
      // See the MSDN page "Graceful Shutdown, Linger Options, and Socket Closure"
      // at http://msdn.microsoft.com/en-us/library/ms738547.aspx
      shutdown(connector_, SD_SEND);
      closesocket(connector_);
#elif defined(OS_POSIX)
      close(connector_);
#endif
    }
  }

  connector_ = kInvalidSocket;
  SetConnState(kConnectStateInvalid);
}

bool TCPConnector::DisConnect() {
	if (!IsConnectInvalidState()) {
    LOG(ERROR) << "DisConnect error!! state is error: " << connect_state_;
		return false;
	}
	
  message_loop_->PostTask(FROM_HERE, base::Bind(&TCPConnector::DoClose, base::Unretained(this), true));

	return true;
}

void TCPConnector::OnConnecting() {
	// Otherwise the connect() is going to complete asynchronously, so watch
	// for its completion.

#if defined(OS_WIN)
  if (!base::MessageLoopForIO2::current()->WatchFileDescriptor(
    connector_, true, base::MessageLoopForIO2::WATCH_READ_WRITE, &watcher_, this)) {
		// 出错如何处理？？
		int os_error = 0;
		os_error = WSAGetLastError();
		LOG(ERROR) << "WatchFileDescriptor failed: " << FormatLastWSAErrorA(os_error);
	}
#else
  if (!base::MessageLoopForIO2::current()->WatchFileDescriptor(
    connector_, true, base::MessageLoopForIO2::WATCH_WRITE, &watcher_, this)) {
		// 出错如何处理？？
		int os_error = errno;
		LOG(ERROR) << "WatchFileDescriptor failed: " << strerror(os_error);
	}

#endif

}

void TCPConnector::OnConnected() {

	if (!IsConnectingState()) {
    LOG(ERROR) << "state error: " << connect_state_;
		return;
	}

	// Get the error that connect() completed with.
	int os_error = 0;
	socklen_t len = sizeof(os_error);
	if (getsockopt(connector_, SOL_SOCKET, SO_ERROR, (char*)&os_error, &len) < 0) {
#if defined(OS_WIN)
		os_error = WSAGetLastError();
#else
		os_error = errno;
#endif
	}

#if defined(OS_WIN)
	// TODO(eroman): Is this check really necessary?
	if (os_error == WSAEINPROGRESS || os_error == WSAEALREADY) {
		NOTREACHED();  // This indicates a bug in libevent or our code.
		return;
	}
#else
	// TODO(eroman): Is this check really necessary?
	if (os_error == EINPROGRESS || os_error == EALREADY) {
		NOTREACHED();  // This indicates a bug in libevent or our code.
		return;
	}
#endif

	watcher_.StopWatchingFileDescriptor();

	int result = MapConnectError(os_error);
	if (result == OK) {
    OnNewConnection(connector_);
	} else {

#if defined(OS_WIN)
		LOG(ERROR) << "Connect host failed: " << FormatLastWSAErrorA(os_error);
#else
		LOG(ERROR) << "Connect host failed: " << strerror( os_error );
#endif

    if (conn_addr_info_.is_reconnect) {
      DoConnect(conn_addr_info_.reconnect_time);
    }

		DoClose(true);
	}
}

// 进行回调
void TCPConnector::OnNewConnection(SOCKET s) {
  DCHECK(delegate_);
  if (delegate_->OnCreateConnection(s)) {
    SetConnState(kConnectStateConnected);
  }
}

void TCPConnector::OnFileCanReadWithoutBlocking(int fd) {
	// MessagePumpLibevent callback, we don't listen for write events
	// so we shouldn't ever reach here.
#if defined(OS_WIN)
	if (IsConnectingState()) {
		OnConnected();
	} 
#else
	NOTREACHED();
#endif
}

void TCPConnector::OnFileCanWriteWithoutBlocking(int fd) {
	if (IsConnectingState()) {
		OnConnected();
	} 
}

}

