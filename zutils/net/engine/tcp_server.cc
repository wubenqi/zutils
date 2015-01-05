// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/engine/tcp_server.h"

#include "base/bind.h"

#include "net/message_loop/message_loop_for_io2.h"

namespace net {

TCPServer::TCPServer(base::MessageLoop* message_loop, IOHandler::Delegate* io_handler_delegate, int reactor_pool_size) 
  : message_loop_(message_loop),
    io_handler_delegate_(io_handler_delegate),
    reactor_pool_(reactor_pool_size),
    acc_(message_loop, this) {

  DCHECK(message_loop);
  DCHECK(message_loop->type()==base::MessageLoop::TYPE_CUSTOM);

  DCHECK(io_handler_delegate);
}

TCPServer::~TCPServer() {
  reactor_pool_.Shutdown();
}

bool TCPServer::Create(const std::string& ip, const std::string& port, bool is_numeric_host_address) {
  bool ret = acc_.Create(ip, port, is_numeric_host_address);
  if (ret && !reactor_pool_.IsEmpty()) {
    ret = reactor_pool_.Start();
  }
  return ret;
}

bool TCPServer::OnCreateConnection(SOCKET s) {
  base::MessageLoop* message_loop = message_loop_;
  if (!reactor_pool_.IsEmpty()) {
    message_loop = reactor_pool_.GetNextMessageLoop();
  }
  
  IOHandlerPtr ih = new IOHandler(message_loop, s, this);
  ih->Create();

//   io_handers_.insert(std::make_pair(ih->io_handler_id(), ih));

  return true;
}

int TCPServer::OnNewConnection(const IOHandlerPtr& ih) {
  return io_handler_delegate_->OnNewConnection(ih);
}

int TCPServer::OnDataReceived(const IOHandlerPtr& ih, IOBuffer* data, base::Time receive_time) {
  return io_handler_delegate_->OnDataReceived(ih, data, receive_time);
}

int TCPServer::OnConnectionClosed(const IOHandlerPtr& ih) {
//   if (message_loop_ == base::MessageLoopForIO2::current()) {
//     OnLoopConnectionClosed(ih);
//   } else {
//     message_loop_->PostTask(FROM_HERE, base::Bind(&TCPServer::OnLoopConnectionClosed, base::Unretained(this), ih));
//   }
  
  int ret = io_handler_delegate_->OnConnectionClosed(ih);

  return ret;
}

}
