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

#include "base/posix/eintr_wrapper.h"
#include "base/bind.h"
#include "base2/base_types.h"

namespace net {

namespace {

const int kReadBufSize = 4096;

}  // namespace

base::StaticAtomicSequenceNumber IOHandler::g_last_sequence_number_;

IOHandler::IOHandler(base::MessageLoop* message_loop, SOCKET socket, Delegate* delegate)
  : socket_(socket)
  , message_loop_(message_loop)
  , delegate_(delegate)
  , read_buf_(64)
  , write_buf_(64)
  , read_wait_state_(NOT_WAITING)
  , write_wait_state_(NOT_WAITING)
  , context_(NULL) {

  DCHECK(message_loop);
  DCHECK(message_loop->type()==base::MessageLoop::TYPE_CUSTOM);

  io_handler_id_ = g_last_sequence_number_.GetNext();
}

IOHandler::~IOHandler() {
  CloseSocket(socket_);
}

// int IOHandler::GetReactorID() {
//   return reactor_->GetReactorID();
// }

void IOHandler::CloseSocket(SOCKET s) {
  if (s && s != kInvalidSocket) {
    // UnwatchSocket();
    read_watcher_.StopWatchingFileDescriptor();
    write_watcher_.StopWatchingFileDescriptor();
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
  if (message_loop_ == base::MessageLoopForIO2::current()) {
    OnCreated();
  } else {
    message_loop_->PostTask(FROM_HERE, base::Bind(&IOHandler::OnCreated, base::Unretained(this)));
  }
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
  base::MessageLoopForIO2::current()->WatchFileDescriptor(
    socket_, true, base::MessageLoopForIO2::WATCH_READ, &read_watcher_, this);
  read_wait_state_ = WAITING_READ;

  struct sockaddr_storage addr_storage;
  socklen_t addr_len = sizeof(addr_storage);
  struct sockaddr* address = reinterpret_cast<struct sockaddr*>(&addr_storage);
  if (!getpeername(socket_, address, &addr_len)) {
    remote_string_ = NetAddressToString(address, addr_len);
    // const struct sockaddr_in* addr = reinterpret_cast<const struct sockaddr_in*>(address);
    // const char* bytes = reinterpret_cast<const char*>(&addr->sin_addr);
    //remote_string_.assign(&bytes[0], &bytes[4]);

  }

  if (delegate_) {
    if (0!=delegate_->OnNewConnection(this)) {
      Close();
    }
  }
}

void IOHandler::Close() {
  if (delegate_) {
    delegate_->OnConnectionClosed(this);
  }

  // OnConnectionClosed();
  CloseSocket(socket_);
  socket_ = kInvalidSocket;

  // io_handler_id_ = 0;
  Release();
}

void IOHandler::Read(base::Time recv_time) {
  DCHECK_EQ(base::MessageLoopForIO2::current(), message_loop_);

  int saved_errno = 0;
  int n = read_buf_.ReadFd(socket_, &saved_errno);
  if (n > 0) {
    int ret = -1;
    if (delegate_) {
      ret = delegate_->OnDataReceived(this, &read_buf_, recv_time);
    }
    // int ret = OnDataReceived(buf, len);
    if (ret!=0) {
      Close();
    }
  } else if (n == 0) {
    Close();
  } else {
    errno = saved_errno;
    LOG(ERROR) << "TcpConnection::handleRead";
    // handleError();
    Close();
  }
}

bool IOHandler::SendData(const void* data, uint32 data_len) {
  SendData(base::StringPiece(static_cast<const char*>(data), data_len));

  return true;
}

bool IOHandler::SendStringPiece(base::StringPiece data) {
  if (base::MessageLoopForIO2::current() == message_loop_) {
    SendInternal(data);
  } else {
    message_loop_->PostTask(FROM_HERE, base::Bind(&IOHandler::SendInternal2, base::Unretained(this), data.as_string()));
  }
  return true;
}

bool IOHandler::SendData(const base::StringPiece& data) {
  if (base::MessageLoopForIO2::current() == message_loop_) {
    SendInternal(data);
  } else {
    message_loop_->PostTask(FROM_HERE, base::Bind(&IOHandler::SendInternal2, base::Unretained(this), data.as_string()));
  }
  return true;
}

bool IOHandler::SendData(net::IOBuffer* data) {
 if (base::MessageLoopForIO2::current() == message_loop_) {
   SendInternal(data->Peek(), data->ReadableBytes());
 } else {
   message_loop_->PostTask(FROM_HERE, base::Bind(&IOHandler::SendInternal2, base::Unretained(this), data->RetrieveAllAsString()));
 }
 return true;
}

void IOHandler::SendInternal2(const std::string& data) {
  return SendInternal(data.c_str(), data.length());
}

void IOHandler::SendInternal(const base::StringPiece& data) {
  return SendInternal(data.data(), data.length());
}

void IOHandler::SendInternal(const void* data, uint32 data_len) {
  DCHECK_EQ(base::MessageLoopForIO2::current(), message_loop_);

  uint32 nwrote = 0;
  size_t remaining = data_len;
  bool fault_error = false;

  if (!IsWriting() && write_buf_.ReadableBytes() == 0) {
    nwrote =  HANDLE_EINTR(send(socket_, static_cast<const char*>(data), data_len, 0));
    if (nwrote >= 0) {
      remaining = data_len - nwrote;
      // if (remaining == 0 && writeCompleteCallback_) {
      //  loop_->queueInLoop(boost::bind(writeCompleteCallback_, shared_from_this()));
      // }
    } else {
      // nwrote < 0
      nwrote = 0;
#if defined(OS_WIN)
      if (WSAGetLastError() != WSAEWOULDBLOCK) {
        LOG(ERROR) << "send failed: WSAGetLastError()==" << WSAGetLastError();
#elif defined(OS_POSIX)
      if (errno != EWOULDBLOCK && errno != EAGAIN) {
        LOG(ERROR) << "send failed: errno==" << errno;
#endif
        fault_error = true;
      }
    }
  }

  DCHECK(remaining <= data_len);
  if (!fault_error && remaining > 0) {
    size_t old_len = write_buf_.ReadableBytes();
//     if (old_len + remaining >= highWaterMark_
//       && oldLen < highWaterMark_
//       && highWaterMarkCallback_)
//     {
//       loop_->queueInLoop(boost::bind(highWaterMarkCallback_, shared_from_this(), oldLen + remaining));
//     }
    write_buf_.Append(static_cast<const char*>(data)+nwrote, remaining);
    if (!IsWriting()) {
      // channel_->enableWriting();
      base::MessageLoopForIO2::current()->WatchFileDescriptor(
        socket_, true, base::MessageLoopForIO2::WATCH_WRITE, &write_watcher_, this);
      // writing_ = true;
      write_wait_state_ = WAITING_WRITE;
    }
  }

  
//     char* send_buf = const_cast<char *>(bytes);
//   int len_left = len;
//   while (true) {
//     int sent = HANDLE_EINTR(send(socket_, send_buf, len_left, 0));
//     if (sent == len_left) {  // A shortcut to avoid extraneous checks.
//       break;
//     }
//     if (sent == kSocketError) {
// #if defined(OS_WIN)
//       if (WSAGetLastError() != WSAEWOULDBLOCK) {
//         LOG(ERROR) << "send failed: WSAGetLastError()==" << WSAGetLastError();
// #elif defined(OS_POSIX)
//       if (errno != EWOULDBLOCK && errno != EAGAIN) {
//         LOG(ERROR) << "send failed: errno==" << errno;
// #endif
//         break;
//       }
//       // Otherwise we would block, and now we have to wait for a retry.
//       // Fall through to PlatformThread::YieldCurrentThread()
//     } else {
//       // sent != len_left according to the shortcut above.
//       // Shift the buffer start and send the remainder after a short while.
//       send_buf += sent;
//       len_left -= sent;
//     }
//     base::PlatformThread::YieldCurrentThread();
//   }
}

void IOHandler::OnWrite() {
  if (IsWriting()) {
    int n =  HANDLE_EINTR(send(socket_, write_buf_.Peek(), write_buf_.ReadableBytes(), 0));
    if (n > 0) {
      write_buf_.Retrieve(n);
      if (write_buf_.ReadableBytes() == 0) {
        write_watcher_.StopWatchingFileDescriptor();
        write_wait_state_ = NOT_WAITING;
//         if (writeCompleteCallback_)
//         {
//           loop_->queueInLoop(boost::bind(writeCompleteCallback_, shared_from_this()));
//         }
//         if (state_ == kDisconnecting)
//         {
//           shutdownInLoop();
//         }
      }
    } else {
      LOG(ERROR) << "TcpConnection::OnWrite";
    }
  } else {
    LOG(ERROR) << "Connection fd = " << socket_ << " is down, no more writing";
  }
}

void IOHandler::OnFileCanReadWithoutBlocking(int fd) {
  if (read_wait_state_ == WAITING_READ) {
    base::Time recv_time = base::Time::Now();
    Read(recv_time);
  } else if (read_wait_state_ == WAITING_CLOSE) {
    // Close() is called by Read() in the Linux case.
    // TODO(erikkay): this seems to get hit multiple times after the close
  }
}

void IOHandler::OnFileCanWriteWithoutBlocking(int fd) {
  if (write_wait_state_ == WAITING_WRITE) {
    OnWrite();
  }
}


}
