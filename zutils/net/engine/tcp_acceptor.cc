// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/engine/tcp_acceptor.h"

#include "base/message_loop.h"
#include "base/bind.h"

#include "net/engine/reactor.h"
#include "net/engine/net_engine_manager.h"

#include "net/engine/default_io_handler_factory.h"

namespace net {

TCPAcceptor::TCPAcceptor(NetEngineManager* engine_manager, IOHandlerFactory* ih_factory, void* user_data/*, IOHandler::IOHandlerDelegate* ih_delegate*/)
	: engine_manager_(engine_manager)
	, ih_factory_(ih_factory)
	, acceptor_(kInvalidSocket)
  , user_data_(user_data) /*
	, ih_delegate_(ih_delegate)*/ {

	// DCHECK(ih_factory);
	if (ih_factory==NULL) {
		// ih_factory_.reset(new DefaultIOHandlerFactory());
	}

	reactor_ = engine_manager->GetReactor();
}

bool TCPAcceptor::Create(const std::string& ip, const std::string& port, bool is_numeric_host_address) {
	int ret = CreateTCPListeningSocket(ip, port, is_numeric_host_address, &acceptor_);
	if ( ret != 0 ) {
		LOG(ERROR) << "Unable to create listening socket for: ret = " << ret
			<< ": " << ip.c_str() << ":"
			<< ip.c_str();
		return false;
	}

	//reactor_->message_loop()->PostTask(FROM_HERE, NewRunnableMethod(
	//	this, &TCPAcceptor::OnCreated));
  reactor_->message_loop()->PostTask(FROM_HERE, base::Bind(&TCPAcceptor::OnCreated, this));

	return true;
}

void TCPAcceptor::OnCreated() {
	WatchSocket(WAITING_ACCEPT);
}

void TCPAcceptor::Accept() {
	SOCKET conn = kInvalidSocket;
	int ret = net::Accept(acceptor_, &conn);
	if (ret != -1) {
		Reactor* reactor = engine_manager_->GetReactor();
		scoped_refptr<IOHandler> ih(ih_factory_->CreateIOHandler(conn, reactor/*, ih_delegate_*/));
    ih->SetUserData(user_data_);
		ih->Create();
	} else {
		// TODO(ibrar): some error handling required here
#if defined(OS_WIN)
		LOG(ERROR) << WSAGetLastError();
#else
   LOG(ERROR) << errno;
#endif
	}
}

void TCPAcceptor::UnwatchSocket() {
	watcher_.StopWatchingFileDescriptor();
}

void TCPAcceptor::WatchSocket(WaitState state) {
	// Implicitly calls StartWatchingFileDescriptor().
	MessageLoopForIO::current()->WatchFileDescriptor(
		acceptor_, true, MessageLoopForIO::WATCH_READ, &watcher_, this);
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

