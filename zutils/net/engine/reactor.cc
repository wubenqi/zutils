// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/engine/reactor.h"

#include "base/bind.h"
#include "base/logging.h"

#include "net/engine/io_handler.h"
#include "net/engine/net_engine_manager.h"


namespace net {

Reactor::Reactor(const std::string& name, int reactor_id, NetEngineManagerDelegate* net_engine_delegate)
	: reactor_id_(reactor_id)
  , base::Thread(name.c_str())
  , timer_(NULL)
  , net_engine_delegate_(net_engine_delegate)
  , user_data_(NULL) {

}

Reactor::~Reactor() {
  
}

bool Reactor::Run() {
	base::Thread::Options options(MessageLoop::TYPE_IO, 0);
	return StartWithOptions(options);
}

void Reactor::Shutdown() {
	Stop();
}

void Reactor::Init() {
  timer_ = new base::Timer(false, false);
  if (net_engine_delegate_) {
    net_engine_delegate_->OnNetEngineCreate(this);
  }
  
}

void Reactor::CleanUp() {
  if (net_engine_delegate_) {
    net_engine_delegate_->OnNetEngineDestroy(this);
    net_engine_delegate_ = NULL;
  }

  if (timer_) {
    delete timer_;
    timer_ = NULL;
  }
}

void Reactor::DoSendSessionData(uint32 io_handler_id, IOBufferPtr io_buffer) {
  message_loop()->PostTask(FROM_HERE, base::Bind(&Reactor::OnSendSessionData, base::Unretained(this), io_handler_id, io_buffer));
}

void Reactor::OnSendSessionData(uint32 io_handler_id, IOBufferPtr io_buffer) {
  IOHandler* ih = io_handlers_.Lookup(io_handler_id);
  if (ih) {
    ih->SendInternal2(io_buffer, io_buffer->data_len());
  } else {
    LOG(ERROR) << "send data error, io_handler_id[" << io_handler_id <<"] not existed!";
  }
}

void Reactor::DoPostTaskData(uint32 io_handler_id, IOBufferPtr io_buffer) {
  message_loop()->PostTask(FROM_HERE, base::Bind(&Reactor::OnTaskData, base::Unretained(this), io_handler_id, io_buffer));
}

void Reactor::OnTaskData(uint32 io_handler_id, IOBufferPtr io_buffer) {
  IOHandler* ih = io_handlers_.Lookup(io_handler_id);
  if (ih) {
    ih->OnTaskDataReceived(io_buffer->data(), io_buffer->data_len());
  } else {
    LOG(ERROR) << "send data error, io_handler_id[" << io_handler_id <<"] not existed!";
  }
}

}

