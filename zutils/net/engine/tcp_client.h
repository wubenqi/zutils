// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_TCP_CLIENT_H_
#define NET_ENGINE_TCP_CLIENT_H_

#include "base/synchronization/lock.h"

#include "net/engine/tcp_connector.h"
#include "net/engine/io_handler.h"

namespace net {

class TCPClient :
  public TCPConnector::Delegate,
  public IOHandler::Delegate {
public:
  TCPClient(base::MessageLoop* message_loop, IOHandler::Delegate* io_handler_delegate);
  ~TCPClient();

  bool Connect(const std::string& ip, const std::string& port, bool is_numeric_host_address);
  bool DisConnect();

  IOHandlerPtr io_handler() const {
    base::AutoLock lock(lock_);
    return io_handler_;
  }

  base::MessageLoop* message_loop() {
    return message_loop_;
  }

protected:
  // TCPConnector::Delegate
  virtual bool OnCreateConnection(SOCKET s);
  virtual bool OnDestroyConnection();

  // IOHandler::Delegate* io_handler_delegate
  virtual int  OnNewConnection(const IOHandlerPtr& ih);
  virtual int  OnDataReceived(const IOHandlerPtr& ih, IOBuffer* data, base::Time receive_time);
  virtual int  OnConnectionClosed(const IOHandlerPtr& ih);

private:
  base::MessageLoop* message_loop_;
  IOHandler::Delegate* io_handler_delegate_;

  mutable base::Lock lock_;
  IOHandlerPtr io_handler_;

  TCPConnector conn_;

  DISALLOW_COPY_AND_ASSIGN(TCPClient);
};

}

#endif
