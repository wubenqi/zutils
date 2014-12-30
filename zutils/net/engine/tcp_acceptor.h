// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_TCP_ACCEPTOR_H_
#define NET_ENGINE_TCP_ACCEPTOR_H_

#include "net/engine/socket_ops.h"
#include "net/message_loop/message_loop_for_io2.h"

namespace net {

class NetEngineManager;
class Reactor;
class TCPAcceptor : public base::MessageLoopForIO2::Watcher {
public:
  class Delegate {
  public:
    virtual bool OnCreateConnection(SOCKET s) { return false; }
  };

  explicit TCPAcceptor(base::MessageLoop* message_loop, Delegate* delegate);
	~TCPAcceptor();

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
  base::MessageLoopForIO2::FileDescriptorWatcher watcher_;
	// Called by MessagePumpLibevent2 when the socket is ready to do I/O
	virtual void OnFileCanReadWithoutBlocking(int fd);
	virtual void OnFileCanWriteWithoutBlocking(int fd);

private:
	// Reactor* reactor_;
	SOCKET acceptor_;
  base::MessageLoop* message_loop_;
  Delegate* delegate_;
};

}

#endif
