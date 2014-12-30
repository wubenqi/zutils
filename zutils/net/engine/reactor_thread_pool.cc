// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/engine/reactor_thread_pool.h"

#include "base/strings/stringprintf.h"

namespace net {

ReactorThreadPool::ReactorThreadPool(int reactor_pool_size, ReactorThread::Delegate* delegate) {
  next_reactor_ = 0;

	reactor_pool_size = reactor_pool_size<0 ? 0 : reactor_pool_size;
  for (int i=0; i<reactor_pool_size; ++i) {
    ReactorThread* reactor = new ReactorThread(i, delegate);
    CHECK(reactor);
    reactor_pool_.push_back(reactor);
  }
}

ReactorThreadPool::~ReactorThreadPool() {
  Shutdown();
}

bool ReactorThreadPool::Start() {
	bool b_rt = true;

  size_t i = 0;
	for (i=0; i<reactor_pool_.size(); ++i) {
		if(!reactor_pool_[i]->Start()) {
			b_rt = false;
			break;
		}
	}

	if (!b_rt) { // 启动Reactor出问题了
		Shutdown();
	}
	return b_rt;
}

void ReactorThreadPool::Shutdown() {
  for (size_t i=0; i<reactor_pool_.size(); ++i) {
    reactor_pool_[i]->Stop();
    delete reactor_pool_[i];
  }
  reactor_pool_.clear();
}

base::MessageLoop* ReactorThreadPool::GetNextMessageLoop() {
  base::MessageLoop* message_loop = NULL;
  if (!reactor_pool_.empty()) {
    if (next_reactor_ == reactor_pool_.size()) next_reactor_ = 0;
    message_loop = reactor_pool_[next_reactor_++]->message_loop();
  }
  return message_loop;
}

}

