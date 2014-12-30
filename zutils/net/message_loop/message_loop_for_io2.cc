// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Edit by wubenqi
// 

#include "net/message_loop/message_loop_for_io2.h"

namespace base {

namespace {

typedef MessagePumpLibevent2 MessagePumpForIO2;

MessagePumpForIO2* ToPumpIO(MessagePump* pump) {
  return static_cast<MessagePumpForIO2*>(pump);
}

}

//------------------------------------------------------------------------------
// MessageLoopForIO

void MessageLoopForIO2::AddIOObserver(
    MessageLoopForIO2::IOObserver* io_observer) {
  ToPumpIO(pump_.get())->AddIOObserver(io_observer);
}

void MessageLoopForIO2::RemoveIOObserver(
    MessageLoopForIO2::IOObserver* io_observer) {
  ToPumpIO(pump_.get())->RemoveIOObserver(io_observer);
}

bool MessageLoopForIO2::WatchFileDescriptor(int fd,
                                           bool persistent,
                                           Mode mode,
                                           FileDescriptorWatcher *controller,
                                           Watcher *delegate) {
  return ToPumpIO(pump_.get())->WatchFileDescriptor(
      fd,
      persistent,
      mode,
      controller,
      delegate);
}

}

