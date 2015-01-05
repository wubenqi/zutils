// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/engine/tcp_client.h"

namespace net {

TCPClient::TCPClient(base::MessageLoop* message_loop, IOHandler::Delegate* io_handler_delegate) 
  : message_loop_(message_loop),
    io_handler_delegate_(io_handler_delegate),
    conn_(message_loop, this) {

  DCHECK(message_loop);
  DCHECK(message_loop->type()==base::MessageLoop::TYPE_CUSTOM);
  DCHECK(io_handler_delegate);
}

TCPClient::~TCPClient() {

}

bool TCPClient::Connect(const std::string& ip, const std::string& port, bool is_numeric_host_address) {
  return conn_.Connect(ip, port, is_numeric_host_address);
}

bool TCPClient::DisConnect() {
  return conn_.DisConnect();
}

bool TCPClient::OnCreateConnection(SOCKET s) {
  IOHandlerPtr ih = new IOHandler(message_loop_, s, this);
  ih->Create();

  {
    base::AutoLock lock(lock_);
    io_handler_ = ih;
  }

  return true;
}

bool TCPClient::OnDestroyConnection() {
  bool ret = false;
  {
    base::AutoLock lock(lock_);
    ret = io_handler_.get()!=NULL;
  }
  return ret;
}

int TCPClient::OnNewConnection(const IOHandlerPtr& ih) {
  return io_handler_delegate_->OnNewConnection(ih);
}

int TCPClient::OnDataReceived(const IOHandlerPtr& ih, IOBuffer* data, base::Time receive_time) {
  return io_handler_delegate_->OnDataReceived(ih, data, receive_time);
}

int TCPClient::OnConnectionClosed(const IOHandlerPtr& ih) {
  int ret = io_handler_delegate_->OnConnectionClosed(ih);

  {
    base::AutoLock lock(lock_);
    io_handler_ = NULL;
  }

  conn_.DoClose(false);

  return ret;
}

}
