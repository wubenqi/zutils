// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_REACROR_THREAD_POOL_H_
#define NET_ENGINE_REACROR_THREAD_POOL_H_
#pragma once

#include <vector>

#include "net/engine/reactor_thread.h"

namespace net {

class ReactorThreadPool {
public:
  explicit ReactorThreadPool(int reactor_pool_size = 0, ReactorThread::Delegate* delegate=NULL);
  ~ReactorThreadPool();

	bool Start();
	void Shutdown();

  bool IsEmpty() const {
    return reactor_pool_.empty();
  }

  // Accept ± π”√
  // 
  base::MessageLoop* GetNextMessageLoop();

private:
	std::vector<ReactorThread*> reactor_pool_;
	size_t next_reactor_;

  DISALLOW_COPY_AND_ASSIGN(ReactorThreadPool);
};

}

#endif
