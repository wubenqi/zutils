// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/engine/net_engine_manager.h"

#include "base/stringprintf.h"
#include "base2/base_types.h"

#include "net/engine/reactor.h"

namespace net {

NetEngineManager::~NetEngineManager() {
  for (size_t i=0; i<reactor_pool_size_; ++i) {
    reactor_pool_[i]->Stop();
    delete reactor_pool_[i];
  }
  reactor_pool_.clear();
}

void NetEngineManager::Initialize(int reactor_pool_size, NetEngineManagerDelegate* net_engine_delegate) {
	reactor_pool_size_ = reactor_pool_size<1?1:static_cast<size_t>(reactor_pool_size);
	std::string reactor_name;
	for (size_t i=0; i<reactor_pool_size_; ++i) {
		reactor_name = StringPrintf("reactor_thread_%d", i);
		Reactor* reactor = new Reactor(reactor_name, i, net_engine_delegate);
    reactor_pool_.push_back(reactor);
	}
}

bool NetEngineManager::Run() {
	bool b_rt = true;
  size_t i = 0;
	for (i=0; i<reactor_pool_size_; ++i) {
		if(!reactor_pool_[i]->Run()) {
			b_rt = false;
			break;
		}
	}

	if (!b_rt) { // 启动Reactor出问题了
		Shutdown();
	}
	return b_rt;
}

void NetEngineManager::Shutdown() {
	for (size_t i=0; i<reactor_pool_size_; ++i) {
		reactor_pool_[i]->Stop();
	}
}

bool NetEngineManager::SendSessionData(uint64 session_id, const char* data, uint32 data_len) {
  uint32 reactor_id = GetHighInt32ByInt64(session_id);
  uint32 ih_id = GetLowInt32ByInt64(session_id);

  bool result = false;
  // 1. 检测是否存在reactor_id
  if (reactor_id>=reactor_pool_.size()) {
    LOG(ERROR) << "reactor_id [" << reactor_id << "]is invalid ";
  } else {
    //IOBufferPtr io_buffer()
    IOBufferPtr io_buffer = new IOBuffer(const_cast<char*>(data), data_len, true);
    reactor_pool_[reactor_id]->DoSendSessionData(GetLowInt32ByInt64(session_id), io_buffer);
    result = true;
  }

  return result;
}

bool NetEngineManager::SendSessionData(uint64 session_id, IOBufferPtr io_buffer) {
  uint32 reactor_id = GetHighInt32ByInt64(session_id);
  uint32 ih_id = GetLowInt32ByInt64(session_id);

  bool result = false;
  // 1. 检测是否存在reactor_id
  if (reactor_id>=reactor_pool_.size()) {
    LOG(ERROR) << "reactor_id [" << reactor_id << "]is invalid ";
  } else {
    reactor_pool_[reactor_id]->DoSendSessionData(GetLowInt32ByInt64(session_id), io_buffer);
    result = true;
  }

  return result;
}

bool NetEngineManager::PostTaskData(uint64 session_id, const char* data, uint32 data_len) {
  uint32 reactor_id = GetHighInt32ByInt64(session_id);
  uint32 ih_id = GetLowInt32ByInt64(session_id);

  bool result = false;
  // 1. 检测是否存在reactor_id
  if (reactor_id>=reactor_pool_.size()) {
    LOG(ERROR) << "reactor_id [" << reactor_id << "]is invalid ";
  } else {
    //IOBufferPtr io_buffer()
    IOBufferPtr io_buffer = new IOBuffer(const_cast<char*>(data), data_len, true);
    reactor_pool_[reactor_id]->DoPostTaskData(GetLowInt32ByInt64(session_id), io_buffer);
    result = true;
  }

  return result;
}

bool NetEngineManager::PostTaskData(uint64 session_id, IOBufferPtr io_buffer) {
  uint32 reactor_id = GetHighInt32ByInt64(session_id);
  uint32 ih_id = GetLowInt32ByInt64(session_id);

  bool result = false;
  // 1. 检测是否存在reactor_id
  if (reactor_id>=reactor_pool_.size()) {
    LOG(ERROR) << "reactor_id [" << reactor_id << "]is invalid ";
  } else {
    reactor_pool_[reactor_id]->DoPostTaskData(GetLowInt32ByInt64(session_id), io_buffer);
    result = true;
  }

  return result;
}

Reactor* NetEngineManager::LookupReactorBySessionID(uint64 session_id) {
  Reactor* reactor = NULL;
  
  uint32 reactor_id = GetHighInt32ByInt64(session_id);
  if (reactor_id<reactor_pool_.size()) {
    reactor = reactor_pool_[reactor_id];
  }

  return reactor;
}

size_t NetEngineManager::GetReactorSize() {
  return reactor_pool_.size();
}

Reactor* NetEngineManager::GetReactor(size_t i) {
  DCHECK_LT(i, reactor_pool_.size());
  return reactor_pool_[i];
}

}

