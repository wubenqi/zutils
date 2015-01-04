// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_TCP_SERVER_H_
#define NET_ENGINE_TCP_SERVER_H_

#include "base/synchronization/lock.h"

#include "net/engine/io_handler.h"
#include "net/engine/reactor_thread_pool.h"
#include "net/engine/io_handler.h"
#include "net/engine/tcp_acceptor.h"

namespace net {

class TCPServer :
  public TCPAcceptor::Delegate,
  public IOHandler::Delegate {
public:
  TCPServer(base::MessageLoop* message_loop, IOHandler::Delegate* io_handler_delegate, int reactor_pool_size = 0);
  ~TCPServer();

  bool Create(const std::string& ip, const std::string& port, bool is_numeric_host_address);

  base::MessageLoop* message_loop() {
    return message_loop_;
  }

protected:
  // TCPAcceptor::Delegate
  virtual bool OnCreateConnection(SOCKET s);

  // IOHandler::Delegate* io_handler_delegate
  virtual int  OnNewConnection(const IOHandlerPtr& ih);
  virtual int  OnDataReceived(const IOHandlerPtr& ih, IOBuffer* data, base::Time receive_time);
  virtual int  OnConnectionClosed(const IOHandlerPtr& ih);

private:
//   void OnLoopConnectionClosed(const IOHandlerPtr& ih) {
//     base::hash_map<int, IOHandlerPtr>::iterator it = io_handers_.find(ih->io_handler_id());
//     if (it!=io_handers_.end()) {
//       io_handers_.erase(it);
//     }
//   }

  base::MessageLoop* message_loop_;
  IOHandler::Delegate* io_handler_delegate_;

  ReactorThreadPool reactor_pool_;

//   base::hash_map<int, IOHandlerPtr> io_handers_;

  TCPAcceptor acc_;

  DISALLOW_COPY_AND_ASSIGN(TCPServer);
};

}

#endif
