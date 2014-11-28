// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_TCP_ACCEPTOR_H_
#define NET_ENGINE_TCP_ACCEPTOR_H_
#pragma once

#include "net/engine/socket_ops.h"
#include "base/message_loop.h"
#include "base/memory/ref_counted.h"

#include "net/engine/io_handler_factory.h"

namespace net {

class NetEngineManager;
class Reactor;
class TCPAcceptor :
	public base::RefCountedThreadSafe<TCPAcceptor>,
	public MessageLoopForIO::Watcher {
public:
	explicit TCPAcceptor(NetEngineManager* engine_manager, IOHandlerFactory* ih_factory = NULL, void* user_data = NULL/*, IOHandler::IOHandlerDelegate* ih_delegate = NULL*/);
	~TCPAcceptor() {
	}

	bool Create(const std::string& ip, const std::string& port, bool is_numeric_host_address);

protected:
	void OnCreated();

	virtual void Accept();
	virtual void Close();

	// Pass any value in case of Windows, because in Windows
	// we are not using state.
	void WatchSocket(WaitState state);
	void UnwatchSocket();

	WaitState wait_state_;
	// The socket's libevent wrapper
	MessageLoopForIO::FileDescriptorWatcher watcher_;
	// Called by MessagePumpLibevent2 when the socket is ready to do I/O
	virtual void OnFileCanReadWithoutBlocking(int fd);
	virtual void OnFileCanWriteWithoutBlocking(int fd);

private:
	Reactor* reactor_;
	SOCKET acceptor_;
	NetEngineManager* engine_manager_;
	//IOHandler::IOHandlerDelegate* ih_delegate_;
	IOHandlerFactory* ih_factory_;
  void* user_data_;

};

typedef scoped_refptr<TCPAcceptor> TCPAcceptorPtr;

}

#endif
