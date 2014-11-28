// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/engine/io_handler.h"

#if defined(OS_WIN)
#include <WS2tcpip.h>
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

#include "base/eintr_wrapper.h"
#include "base/message_loop.h"
#include "base/bind.h"
#include "base2/base_types.h"

#include "net/engine/reactor.h"

namespace net {

namespace {

const int kReadBufSize = 4096;

}  // namespace

IOHandler::IOHandler(SOCKET socket, Reactor* reactor/*, IOHandlerDelegate* ih_delegate*/)
  : socket_(socket)
  , reactor_(reactor)
  , ih_call_back_(NULL)
  , wait_state_(NOT_WAITING)
  , session_id_(kInvalidSessionID) {

    //ih_delegates_.push_back(ih_delegate);
}

IOHandler::~IOHandler() {
  CloseSocket(socket_);
}

int IOHandler::GetReactorID() {
  return reactor_->GetReactorID();
}

void IOHandler::CloseSocket(SOCKET s) {
  if (s && s != kInvalidSocket) {
    UnwatchSocket();
#if defined(OS_WIN)
    // Note: don't use CancelIo to cancel pending IO because it doesn't work
    // when there is a Winsock layered service provider.

    // In most socket implementations, closing a socket results in a graceful
    // connection shutdown, but in Winsock we have to call shutdown explicitly.
    // See the MSDN page "Graceful Shutdown, Linger Options, and Socket Closure"
    // at http://msdn.microsoft.com/en-us/library/ms738547.aspx
    shutdown(s, SD_SEND);
    closesocket(s);
#elif defined(OS_POSIX)
    close(s);
#endif
  }
}

void IOHandler::Create() {
  AddRef();
  reactor_->message_loop()->PostTask(FROM_HERE, base::Bind(&IOHandler::OnCreated, base::Unretained(this)));
}

namespace {
//#ifndef INET6_ADDRSTRLEN
//#define INET6_ADDRSTRLEN 65
//#endif
std::string NetAddressToString(const struct sockaddr* net_address, socklen_t address_len) {
#if defined(OS_WIN)
    // EnsureWinsockInit();
#endif

    // This buffer is large enough to fit the biggest IPv6 string.
    char buffer[INET6_ADDRSTRLEN];

    int result = getnameinfo(net_address, address_len, buffer, sizeof(buffer),
      NULL, 0, NI_NUMERICHOST);

    if (result != 0) {
      DVLOG(1) << "getnameinfo() failed with " << result << ": "
        << gai_strerror(result);
      buffer[0] = '\0';
    }
    return std::string(buffer);
}
}

void IOHandler::OnCreated() {
  WatchSocket(WAITING_READ);

  session_id_ = MakeInt64(reactor_->AddIOHandler(this), reactor_->GetReactorID());

  struct sockaddr_storage addr_storage;
  socklen_t addr_len = sizeof(addr_storage);
  struct sockaddr* address = reinterpret_cast<struct sockaddr*>(&addr_storage);
  if (!getpeername(socket_, address, &addr_len)) {
    remote_string_ = NetAddressToString(address, addr_len);
    // const struct sockaddr_in* addr = reinterpret_cast<const struct sockaddr_in*>(address);
    // const char* bytes = reinterpret_cast<const char*>(&addr->sin_addr);
    //remote_string_.assign(&bytes[0], &bytes[4]);

  }

  if (ih_call_back_) {
    ih_call_back_->OnNewConnection(this);
  }

  OnNewConnection();

  //std::list<IOHandlerDelegate *>::iterator it;
  //for(it=ih_delegates_.begin(); it!=ih_delegates_.end(); ++it) {
  //	if(*it)
  //		(*it)->OnNewConnection(this);
  //}
}

void IOHandler::Close() {
  //std::list<IOHandlerDelegate *>::iterator it;
  //for(it=ih_delegates_.begin(); it!=ih_delegates_.end(); ++it) {
  //	if(*it)
  //		(*it)->OnConnectionClosed(this);
  //}

  if (ih_call_back_) {
    ih_call_back_->OnConnectionClosed(this);
  }

  OnConnectionClosed();
  CloseSocket(socket_);
  socket_ = kInvalidSocket;

  reactor_->Remove(GetLowInt32ByInt64(session_id_));
  session_id_ = 0;
  Release();
}

void IOHandler::Read() {
  char buf[kReadBufSize + 1];  // +1 for null termination
  int len;
  do {
    len = HANDLE_EINTR(recv(socket_, buf, kReadBufSize, 0));
    if (len == kSocketError) {
#if defined(OS_WIN)
      int err = WSAGetLastError();
      if (err == WSAEWOULDBLOCK) {
        LOG(ERROR) << "recv failed: WSAGetLastError()==" << WSAGetLastError();
#elif defined(OS_POSIX)
      if (errno == EWOULDBLOCK || errno == EAGAIN) {
        LOG(ERROR) << "recv failed: WSAGetLastError()==" << errno;
#endif

        break;
      } else {
        // TODO(ibrar): some error handling required here
        Close();
        break;
      }
    } else if (len == 0) {
      // In Windows, Close() is called by OnObjectSignaled.  In POSIX, we need
      // to call it here.
      Close();
      break;
    } else {
      // TODO(ibrar): maybe change DidRead to take a length instead
      DCHECK(len > 0 && len <= kReadBufSize);
      buf[len] = 0;  // already create a buffer with +1 length

      int ret = OnDataReceived(buf, len);
      if (ret!=0) {
        Close();
        break;
      }

      // socket_delegate_->DidRead(this, buf, len);
      /*
      std::list<IOHandlerDelegate *>::iterator it;
      for(it=ih_delegates_.begin(); it!=ih_delegates_.end(); ++it) {
        if(*it) {
          int ret = (*it)->OnDataReceived(this, buf, len);
          if (ret!=0) {
            Close();
          }
        }
      }
      */
    }
  } while (len == kReadBufSize);


  /*
  SendInternal(buf, len+1);
  Close();
  */
}

void IOHandler::AsyncSendIOBuffer(IOBufferPtr io_buffer) {
  reactor_->message_loop()->PostTask(FROM_HERE, base::Bind(&IOHandler::SendInternal2, base::Unretained(this), io_buffer, io_buffer->data_len()));
}

void IOHandler::AsyncSend(const char* io_buffer, uint32 io_buffer_len) {
  scoped_refptr<IOBuffer> data = new IOBuffer(io_buffer_len);
  memcpy(data->data(), io_buffer, io_buffer_len);
  reactor_->message_loop()->PostTask(FROM_HERE, base::Bind(&IOHandler::SendInternal2, base::Unretained(this), data, io_buffer_len));

  //reactor_->message_loop()->PostTask(FROM_HERE, NewRunnableMethod(
  //	this, &IOHandler::SendInternal2, data, io_buffer_len));
}

void IOHandler::SendInternal2(scoped_refptr<IOBuffer> io_buffer, uint32 io_buffer_len) {
  SendInternal(io_buffer->data(), io_buffer_len);
}

void IOHandler::SendInternal(const char* bytes, int len) {
  char* send_buf = const_cast<char *>(bytes);
  int len_left = len;
  while (true) {
    int sent = HANDLE_EINTR(send(socket_, send_buf, len_left, 0));
    if (sent == len_left) {  // A shortcut to avoid extraneous checks.
      break;
    }
    if (sent == kSocketError) {
#if defined(OS_WIN)
      if (WSAGetLastError() != WSAEWOULDBLOCK) {
        LOG(ERROR) << "send failed: WSAGetLastError()==" << WSAGetLastError();
#elif defined(OS_POSIX)
      if (errno != EWOULDBLOCK && errno != EAGAIN) {
        LOG(ERROR) << "send failed: errno==" << errno;
#endif
        break;
      }
      // Otherwise we would block, and now we have to wait for a retry.
      // Fall through to PlatformThread::YieldCurrentThread()
    } else {
      // sent != len_left according to the shortcut above.
      // Shift the buffer start and send the remainder after a short while.
      send_buf += sent;
      len_left -= sent;
    }
    base::PlatformThread::YieldCurrentThread();
  }
}

void IOHandler::Send(const char* bytes, int len) {
  SendInternal(bytes, len);
}

void IOHandler::Send(const std::string& str) {
  SendInternal(str.data(), static_cast<int>(str.length()));
}

void IOHandler::UnwatchSocket() {
  watcher_.StopWatchingFileDescriptor();
}

void IOHandler::WatchSocket(WaitState state) {
  // Implicitly calls StartWatchingFileDescriptor().
  MessageLoopForIO::current()->WatchFileDescriptor(
    socket_, true, MessageLoopForIO::WATCH_READ, &watcher_, this);
  wait_state_ = state;
}


void IOHandler::OnFileCanReadWithoutBlocking(int fd) {
  if (wait_state_ == WAITING_READ) {
    Read();
  }
  if (wait_state_ == WAITING_CLOSE) {
    // Close() is called by Read() in the Linux case.
    // TODO(erikkay): this seems to get hit multiple times after the close
  }
}

void IOHandler::OnFileCanWriteWithoutBlocking(int fd) {
  // MessagePumpLibevent callback, we don't listen for write events
  // so we shouldn't ever reach here.
  NOTREACHED();
}


}
