// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//


#ifndef NET_ENGINE_NET_ENGINE_MANAGER_H_
#define NET_ENGINE_NET_ENGINE_MANAGER_H_
#pragma once

#include <vector>

#include "base/basictypes.h"
#include "base/memory/singleton.h"
#include "base/atomic_sequence_num.h"

#include "net/base/io_buffer.h"

namespace net {

class Reactor;
class NetEngineManagerDelegate {
public:
  virtual ~NetEngineManagerDelegate() {}
  virtual void OnNetEngineCreate(Reactor* reactor) {}
  virtual void OnNetEngineDestroy(Reactor* reactor) {}
};

class NetEngineManager {
public:
  static NetEngineManager* GetInstance() {
    return Singleton<NetEngineManager>::get();
  }

  void Initialize(int reactor_pool_size, NetEngineManagerDelegate* net_engine_delegate=NULL);

	bool Run();
	void Shutdown();

	Reactor* GetReactor() {
    if (reactor_pool_size_ == 1) {
      return reactor_pool_[0];
    } else {
      uint32 next_reactor = static_cast<uint32>(next_reactor_.GetNext());
      return reactor_pool_[next_reactor%reactor_pool_size_];
    }
	}

  bool SendSessionData(uint64 session_id, const char* data, uint32 data_len);
  bool SendSessionData(uint64 session_id, IOBufferPtr io_buffer);

  bool PostTaskData(uint64 session_id, const char* data, uint32 data_len);
  bool PostTaskData(uint64 session_id, IOBufferPtr io_buffer);

  Reactor* LookupReactorBySessionID(uint64 session_id);

  size_t GetReactorSize();
  Reactor* GetReactor(size_t i);

private:
  friend struct DefaultSingletonTraits<NetEngineManager>;

  NetEngineManager() {
    reactor_pool_size_ = 0;
  }

  ~NetEngineManager();

	std::vector<Reactor*> reactor_pool_;
	size_t reactor_pool_size_;
	base::AtomicSequenceNumber next_reactor_;

  DISALLOW_COPY_AND_ASSIGN(NetEngineManager);
};

}

#endif
