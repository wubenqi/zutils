// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/engine/tcp_acceptor.h"

namespace net {

TCPAcceptor::TCPAcceptor(base::MessageLoop* message_loop, TCPAcceptor::Delegate* delegate)
	: acceptor_(kInvalidSocket),
    message_loop_(message_loop),
    delegate_(delegate) {

	DCHECK(message_loop);
  DCHECK(message_loop->type()==base::MessageLoop::TYPE_CUSTOM);

  DCHECK(delegate);
}

TCPAcceptor::~TCPAcceptor() {
#if defined(OS_WIN)
  closesocket(acceptor_);
#elif defined(OS_POSIX)
  close(acceptor_);
#endif
}

bool TCPAcceptor::Create(const std::string& ip, const std::string& port, bool is_numeric_host_address) {
	int ret = CreateTCPListeningSocket(ip, port, is_numeric_host_address, &acceptor_);
	if ( ret != 0 ) {
		LOG(ERROR) << "Unable to create listening socket for: ret = " << ret
			<< ": " << ip.c_str() << ":"
			<< ip.c_str();
		return false;
	}

  OnCreated();
	return true;
}

void TCPAcceptor::OnCreated() {
	WatchSocket(WAITING_ACCEPT);
}

void TCPAcceptor::Accept() {
	SOCKET conn = kInvalidSocket;
	int ret = net::Accept(acceptor_, &conn);
	if (ret != -1) {
    delegate_->OnCreateConnection(conn);
	} else {
		// TODO(ibrar): some error handling required here
#if defined(OS_WIN)
		LOG(ERROR) << WSAGetLastError();
#else
   // LOG(ERROR) << errno;
#endif
	}
}

void TCPAcceptor::UnwatchSocket() {
	watcher_.StopWatchingFileDescriptor();
}

void TCPAcceptor::WatchSocket(WaitState state) {
	// Implicitly calls StartWatchingFileDescriptor().
  base::MessageLoopForIO2::current()->WatchFileDescriptor(
    acceptor_, true, base::MessageLoopForIO2::WATCH_READ, &watcher_, this);
	wait_state_ = state;
}

void TCPAcceptor::Close() {
	if (wait_state_ == WAITING_CLOSE)
		return;
	wait_state_ = WAITING_CLOSE;
}

void TCPAcceptor::OnFileCanReadWithoutBlocking(int fd) {
	if (wait_state_ == WAITING_ACCEPT) {
		Accept();
	}
	if (wait_state_ == WAITING_CLOSE) {
		// Close() is called by Read() in the Linux case.
		// TODO(erikkay): this seems to get hit multiple times after the close
	}
}

void TCPAcceptor::OnFileCanWriteWithoutBlocking(int fd) {
	// MessagePumpLibevent callback, we don't listen for write events
	// so we shouldn't ever reach here.
	NOTREACHED();
}


}

