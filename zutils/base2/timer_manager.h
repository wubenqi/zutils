// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef BASE2_TIMER_MANAGER_H_
#define BASE2_TIMER_MANAGER_H_
#pragma once

#include "base/containers/hash_tables.h"
#include "base/message_loop/message_loop.h"
#include "base/timer/timer.h"
#include "base2/shared_ptr.h"

class TimerManager {
public:
  class Delegate {
  public:
    virtual ~Delegate() {}
    virtual void OnTimer(uint32 timer_id, int tm) {}
  };

  TimerManager(Delegate* delegate, base::MessageLoop* message_loop)
    : message_loop_(message_loop)
    , delegate_(delegate) {
  }

  ~TimerManager() {
    Destroy();
  }

  void Destroy();

  void SetTimer(uint32 timer_id, int tm);
  void KillTimer(uint32 timer_id);

private:
  void handle_timeout(unsigned int timer_id, int tm);
  typedef shared_ptr<base::Timer> TimerPtr;

  base::MessageLoop* message_loop_;
  //boost::asio::io_service& io_service_;
  typedef base::hash_map<uint32, TimerPtr> TimerMap;
  TimerMap timers_;
  Delegate* delegate_;
};

#endif
