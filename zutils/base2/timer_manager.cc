// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "base2/timer_manager.h"

#include "base/message_loop/message_loop.h"
#include "base/bind.h"

#include "base/logging.h"

void TimerManager::Destroy() {
  TimerMap::iterator it;
  for (it = timers_.begin(); it!=timers_.end(); ++it){
    it->second->Stop();
  }
  timers_.clear();
}

void TimerManager::SetTimer(uint32 timer_id, int tm) {
  TimerMap::iterator it;
  if ((it=timers_.find(timer_id))==timers_.end()){
    TimerPtr timer2(new base::Timer(false, false));
    timers_.insert(std::make_pair(timer_id, timer2));
    timer2->Start(FROM_HERE, base::TimeDelta::FromMilliseconds(tm), base::Bind(&TimerManager::handle_timeout, base::Unretained(this), timer_id, tm));
  } else {
    LOG(ERROR) <<"[TimerManager]: timer_id(" << timer_id << ") is existed!!!";
  }
}

void TimerManager::KillTimer(uint32 timer_id) {
  TimerMap::iterator it;
  if ((it=timers_.find(timer_id))!=timers_.end()){
    it->second->Stop();
    timers_.erase(it);
  }
}

void TimerManager::handle_timeout(unsigned int timer_id, int tm) {
  LOG(INFO) << "TimerManager::handle_timeout()";
  TimerMap::iterator it;
  if ((it=timers_.find(timer_id))!=timers_.end()){
    timers_.erase(it);
  }
  if (delegate_) {
    delegate_->OnTimer(timer_id, tm);
  }
}
