// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/engine/reactor_thread.h"

#include "base/memory/scoped_ptr.h"
#include "base/strings/stringprintf.h"
#include "base/bind.h"
#include "base/logging.h"

#include "net/message_loop/message_pump_libevent2.h"

namespace net {

namespace {
// for base::Thread::Options::message_pump_factory
scoped_ptr<base::MessagePump> MessagePumpLibevent2Factory() {
  return scoped_ptr<base::MessagePump>(new base::MessagePumpLibevent2());
}

}

ReactorThread::ReactorThread(int reactor_id, Delegate* delegate)
  : reactor_id_(reactor_id),
    base::Thread(base::StringPrintf("reactor_thread_%d", reactor_id)),
    delegate_(delegate) {

}

ReactorThread::~ReactorThread() {
  Shutdown();
}

bool ReactorThread::Start() {
  base::Thread::Options options;
  options.message_loop_type = base::MessageLoop::TYPE_CUSTOM;
  options.message_pump_factory = base::Bind(&MessagePumpLibevent2Factory);
  return StartWithOptions(options);
}

void ReactorThread::Shutdown() {
	Stop();
}

void ReactorThread::Init() {
  if (delegate_) {
    delegate_->OnReactorCreate(this);
  }
}

void ReactorThread::CleanUp() {
  if (delegate_) {
    delegate_->OnReactorDestroy(this);
    delegate_ = NULL;
  }
}

}

