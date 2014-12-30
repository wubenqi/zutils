// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Edit by wubenqi
// 

#ifndef NET_MESSAGE_LOOP_MESSAGE_LOOP_FOR_IO2_H_
#define NET_MESSAGE_LOOP_MESSAGE_LOOP_FOR_IO2_H_

#include "base/message_loop/message_loop.h"

#include "net/message_loop/message_pump_libevent2.h"

namespace base {

//-----------------------------------------------------------------------------
// MessageLoopForIO extends MessageLoop with methods that are particular to a
// MessageLoop instantiated with TYPE_IO.
//
// This class is typically used like so:
//   MessageLoopForIO::current()->...call some method...
//
class MessageLoopForIO2 : public MessageLoop {
 public:
  MessageLoopForIO2() : MessageLoop(TYPE_CUSTOM) {
  }

  // Returns the MessageLoopForIO of the current thread.
  static MessageLoopForIO2* current() {
    MessageLoop* loop = MessageLoop::current();
    DCHECK_EQ(MessageLoop::TYPE_CUSTOM, loop->type());
    return static_cast<MessageLoopForIO2*>(loop);
  }

  static bool IsCurrent() {
    MessageLoop* loop = MessageLoop::current();
    return loop && loop->type() == MessageLoop::TYPE_CUSTOM;
  }

  typedef MessagePumpLibevent2::Watcher Watcher;
  typedef MessagePumpLibevent2::FileDescriptorWatcher
    FileDescriptorWatcher;
  typedef MessagePumpLibevent2::IOObserver IOObserver;

  enum Mode {
    WATCH_READ = MessagePumpLibevent2::WATCH_READ,
    WATCH_WRITE = MessagePumpLibevent2::WATCH_WRITE,
    WATCH_READ_WRITE = MessagePumpLibevent2::WATCH_READ_WRITE
  };

  void AddIOObserver(IOObserver* io_observer);
  void RemoveIOObserver(IOObserver* io_observer);

  bool WatchFileDescriptor(int fd,
    bool persistent,
    Mode mode,
    FileDescriptorWatcher *controller,
    Watcher *delegate);

};

// Do not add any member variables to MessageLoopForIO!  This is important b/c
// MessageLoopForIO is often allocated via MessageLoop(TYPE_IO).  Any extra
// data that you need should be stored on the MessageLoop's pump_ instance.
COMPILE_ASSERT(sizeof(MessageLoop) == sizeof(MessageLoopForIO2),
               MessageLoopForIO2_should_not_have_extra_member_variables);

}

#endif
