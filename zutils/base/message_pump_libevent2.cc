// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "base/message_pump_libevent2.h"

#include <errno.h>
#include <fcntl.h>
//#include <unistd.h>

#include "base/auto_reset.h"
#include "base/compiler_specific.h"
#include "base/eintr_wrapper.h"
#include "base/logging.h"
#if defined(OS_MACOSX)
#include "base/mac/scoped_nsautorelease_pool.h"
#endif
#include "base/memory/scoped_ptr.h"
#include "base/observer_list.h"
#include "base/time.h"
#include "base/stringprintf.h"

#if defined(OS_WIN)
#include <winsock2.h>
#endif

#include "third_party/libevent/event.h"

#if defined(OS_MACOSX)
#include "base/mac/scoped_nsautorelease_pool.h"
#endif

// Lifecycle of struct event
// Libevent uses two main data structures:
// struct event_base (of which there is one per message pump), and
// struct event (of which there is roughly one per socket).
// The socket's struct event is created in
// MessagePumpLibevent2::WatchFileDescriptor(),
// is owned by the FileDescriptorWatcher, and is destroyed in
// StopWatchingFileDescriptor().
// It is moved into and out of lists in struct event_base by
// the libevent functions event_add() and event_del().
//
// TODO(dkegel):
// At the moment bad things happen if a FileDescriptorWatcher
// is active after its MessagePumpLibevent2 has been destroyed.
// See MessageLoopTest.FileDescriptorWatcherOutlivesMessageLoop
// Not clear yet whether that situation occurs in practice,
// but if it does, we need to fix it.

namespace base {

// Return 0 on success
// Too small a function to bother putting in a library?
#if !defined(OS_WIN)
static int SetNonBlocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1)
    flags = 0;
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
#else
static int SetNonBlocking(int fd) {
	 unsigned long no_block = 1;
	 int ioctl_return = ioctlsocket(fd, FIONBIO, &no_block);
	 return ioctl_return;
}
#endif

#if defined(OS_WIN)
int SocketPairWin32(int socks[2]) {
  struct sockaddr_in addr;
  SOCKET listener;
  int e;
  int addrlen = sizeof(addr);
  DWORD flags = 0;

  if (socks == 0) {
    WSASetLastError(WSAEINVAL);
    return SOCKET_ERROR;
  }

  socks[0] = socks[1] = INVALID_SOCKET;
  if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) 
    return SOCKET_ERROR;

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(0x7f000001);
  addr.sin_port = 0;

  e = bind(listener, (const struct sockaddr*) &addr, sizeof(addr));
  if (e == SOCKET_ERROR) {
    e = WSAGetLastError();
    closesocket(listener);
    WSASetLastError(e);
    return SOCKET_ERROR;
  }
  e = getsockname(listener, (struct sockaddr*) &addr, &addrlen);
  if (e == SOCKET_ERROR) {
    e = WSAGetLastError();
    closesocket(listener);
    WSASetLastError(e);
    return SOCKET_ERROR;
  }

  do {
    if (listen(listener, 1) == SOCKET_ERROR) break;
    if ((socks[0] = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, flags)) == INVALID_SOCKET) break;
    if (connect(socks[0], (const struct sockaddr*) &addr, sizeof(addr)) == SOCKET_ERROR) break;
    if ((socks[1] = accept(listener, NULL, NULL)) == INVALID_SOCKET) break;
    closesocket(listener);
    return 0;
  } while (0);

  e = WSAGetLastError();
  closesocket(listener);
  closesocket(socks[0]);
  closesocket(socks[1]);
  WSASetLastError(e);
  return SOCKET_ERROR;
}

static std::string FormatGetLastWSAErrorA(unsigned messageid) {
  char* string_buffer = NULL;
  unsigned string_length = ::FormatMessageA(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS, NULL, messageid, 0,
    reinterpret_cast<char *>(&string_buffer), 0, NULL);

  std::string formatted_string;
  if (string_buffer) {
    formatted_string = string_buffer;
    LocalFree(reinterpret_cast<HLOCAL>(string_buffer));
  } else {
    // The formating failed. simply convert the message value into a string.
    base::SStringPrintf(&formatted_string, "message number %d", messageid);
  }
  return formatted_string;
}

#endif

MessagePumpLibevent2::FileDescriptorWatcher::FileDescriptorWatcher()
    : event_(NULL),
      pump_(NULL),
      watcher_(NULL),
      ALLOW_THIS_IN_INITIALIZER_LIST(weak_factory_(this)) {
}

MessagePumpLibevent2::FileDescriptorWatcher::~FileDescriptorWatcher() {
  if (event_) {
    StopWatchingFileDescriptor();
  }
}

bool MessagePumpLibevent2::FileDescriptorWatcher::StopWatchingFileDescriptor() {
  event* e = ReleaseEvent();
  if (e == NULL)
    return true;

  // event_del() is a no-op if the event isn't active.
  int rv = event_del(e);
  delete e;
  pump_ = NULL;
  watcher_ = NULL;
  return (rv == 0);
}

void MessagePumpLibevent2::FileDescriptorWatcher::Init(event *e) {
  DCHECK(e);
  DCHECK(!event_);

  event_ = e;
}

event *MessagePumpLibevent2::FileDescriptorWatcher::ReleaseEvent() {
  struct event *e = event_;
  event_ = NULL;
  return e;
}

void MessagePumpLibevent2::FileDescriptorWatcher::OnFileCanReadWithoutBlocking(
    int fd, MessagePumpLibevent2* pump) {
  // Since OnFileCanWriteWithoutBlocking() gets called first, it can stop
  // watching the file descriptor.
  if (!watcher_)
    return;
  pump->WillProcessIOEvent();
  watcher_->OnFileCanReadWithoutBlocking(fd);
  pump->DidProcessIOEvent();
}

void MessagePumpLibevent2::FileDescriptorWatcher::OnFileCanWriteWithoutBlocking(
    int fd, MessagePumpLibevent2* pump) {
  DCHECK(watcher_);
  pump->WillProcessIOEvent();
  watcher_->OnFileCanWriteWithoutBlocking(fd);
  pump->DidProcessIOEvent();
}

MessagePumpLibevent2::MessagePumpLibevent2()
    : keep_running_(true),
      in_run_(false),
      processed_io_events_(false),
      event_base_(event_base_new()),
      wakeup_pipe_in_(-1),
      wakeup_pipe_out_(-1) {
  if (!Init())
     NOTREACHED();
}

MessagePumpLibevent2::~MessagePumpLibevent2() {
  DCHECK(wakeup_event_);
  DCHECK(event_base_);
  event_del(wakeup_event_);
  delete wakeup_event_;
  if (wakeup_pipe_in_ >= 0) {
#if defined(OS_WIN)
    if (HANDLE_EINTR(closesocket(wakeup_pipe_in_)) < 0)
#else
    if (HANDLE_EINTR(close(wakeup_pipe_in_)) < 0)
#endif
      DPLOG(ERROR) << "close";
  }
  if (wakeup_pipe_out_ >= 0) {
#if defined(OS_WIN)
    if (HANDLE_EINTR(closesocket(wakeup_pipe_out_)) < 0)
#else
    if (HANDLE_EINTR(close(wakeup_pipe_out_)) < 0)
#endif
      DPLOG(ERROR) << "close";
  }
  event_base_free(event_base_);
}

bool MessagePumpLibevent2::WatchFileDescriptor(int fd,
                                              bool persistent,
                                              Mode mode,
                                              FileDescriptorWatcher *controller,
                                              Watcher *delegate) {
  DCHECK_GE(fd, 0);
  DCHECK(controller);
  DCHECK(delegate);
  DCHECK(mode == WATCH_READ || mode == WATCH_WRITE || mode == WATCH_READ_WRITE);
  // WatchFileDescriptor should be called on the pump thread. It is not
  // threadsafe, and your watcher may never be registered.
  DCHECK(watch_file_descriptor_caller_checker_.CalledOnValidThread());

  int event_mask = persistent ? EV_PERSIST : 0;
  if ((mode & WATCH_READ) != 0) {
    event_mask |= EV_READ;
  }
  if ((mode & WATCH_WRITE) != 0) {
    event_mask |= EV_WRITE;
  }

  scoped_ptr<event> evt(controller->ReleaseEvent());
  if (evt.get() == NULL) {
    // Ownership is transferred to the controller.
    evt.reset(new event);
  } else {
    // Make sure we don't pick up any funky internal libevent masks.
    int old_interest_mask = evt.get()->ev_events &
        (EV_READ | EV_WRITE | EV_PERSIST);

    // Combine old/new event masks.
    event_mask |= old_interest_mask;

    // Must disarm the event before we can reuse it.
    event_del(evt.get());

    // It's illegal to use this function to listen on 2 separate fds with the
    // same |controller|.
    if (EVENT_FD(evt.get()) != fd) {
      NOTREACHED() << "FDs don't match" << EVENT_FD(evt.get()) << "!=" << fd;
      return false;
    }
  }

  // Set current interest mask and message pump for this event.
  event_set(evt.get(), fd, event_mask, OnLibeventNotification, controller);

  // Tell libevent which message pump this socket will belong to when we add it.
  if (event_base_set(event_base_, evt.get()) != 0) {
    return false;
  }

  // Add this socket to the list of monitored sockets.
  if (event_add(evt.get(), NULL) != 0) {
    return false;
  }

  // Transfer ownership of evt to controller.
  controller->Init(evt.release());

  controller->set_watcher(delegate);
  controller->set_pump(this);

  return true;
}

void MessagePumpLibevent2::AddIOObserver(IOObserver *obs) {
  io_observers_.AddObserver(obs);
}

void MessagePumpLibevent2::RemoveIOObserver(IOObserver *obs) {
  io_observers_.RemoveObserver(obs);
}

// Tell libevent to break out of inner loop.
static void timer_callback(int fd, short events, void *context)
{
  event_base_loopbreak((struct event_base *)context);
}

// Reentrant!
void MessagePumpLibevent2::Run(Delegate* delegate) {
  DCHECK(keep_running_) << "Quit must have been called outside of Run!";
  AutoReset<bool> auto_reset_in_run(&in_run_, true);

  // event_base_loopexit() + EVLOOP_ONCE is leaky, see http://crbug.com/25641.
  // Instead, make our own timer and reuse it on each call to event_base_loop().
  scoped_ptr<event> timer_event(new event);

  for (;;) {
#if defined(OS_MACOSX)
    mac::ScopedNSAutoreleasePool autorelease_pool;
#endif

    bool did_work = delegate->DoWork();
    if (!keep_running_)
      break;

    event_base_loop(event_base_, EVLOOP_NONBLOCK);
    did_work |= processed_io_events_;
    processed_io_events_ = false;
    if (!keep_running_)
      break;

    did_work |= delegate->DoDelayedWork(&delayed_work_time_);
    if (!keep_running_)
      break;

    if (did_work)
      continue;

    did_work = delegate->DoIdleWork();
    if (!keep_running_)
      break;

    if (did_work)
      continue;

    // EVLOOP_ONCE tells libevent to only block once,
    // but to service all pending events when it wakes up.
    if (delayed_work_time_.is_null()) {
      event_base_loop(event_base_, EVLOOP_ONCE);
    } else {
      TimeDelta delay = delayed_work_time_ - TimeTicks::Now();
      if (delay > TimeDelta()) {
        struct timeval poll_tv;
        poll_tv.tv_sec = delay.InSeconds();
        poll_tv.tv_usec = delay.InMicroseconds() % Time::kMicrosecondsPerSecond;
        event_set(timer_event.get(), -1, 0, timer_callback, event_base_);
        event_base_set(event_base_, timer_event.get());
        event_add(timer_event.get(), &poll_tv);
        event_base_loop(event_base_, EVLOOP_ONCE);
        event_del(timer_event.get());
      } else {
        // It looks like delayed_work_time_ indicates a time in the past, so we
        // need to call DoDelayedWork now.
        delayed_work_time_ = TimeTicks();
      }
    }
  }

  keep_running_ = true;
}

void MessagePumpLibevent2::Quit() {
  DCHECK(in_run_);
  // Tell both libevent and Run that they should break out of their loops.
  keep_running_ = false;
  ScheduleWork();
}

void MessagePumpLibevent2::ScheduleWork() {
  // Tell libevent (in a threadsafe way) that it should break out of its loop.
  char buf = 0;
#if !defined(OS_WIN)
  int nwrite = HANDLE_EINTR(write(wakeup_pipe_in_, &buf, 1));
  DCHECK(nwrite == 1 || errno == EAGAIN)
      << "[nwrite:" << nwrite << "] [errno:" << errno << "]";
#else
  int nwrite = HANDLE_EINTR(send(wakeup_pipe_in_, &buf, 1, 0));
  DCHECK(nwrite == 1 || WSAGetLastError() == WSAEWOULDBLOCK)
      << "[nwrite:" << nwrite << "] [errno:" << errno << "]";
#endif
}

void MessagePumpLibevent2::ScheduleDelayedWork(
    const TimeTicks& delayed_work_time) {
  // We know that we can't be blocked on Wait right now since this method can
  // only be called on the same thread as Run, so we only need to update our
  // record of how long to sleep when we do sleep.
  delayed_work_time_ = delayed_work_time;
}

void MessagePumpLibevent2::WillProcessIOEvent() {
  FOR_EACH_OBSERVER(IOObserver, io_observers_, WillProcessIOEvent());
}

void MessagePumpLibevent2::DidProcessIOEvent() {
  FOR_EACH_OBSERVER(IOObserver, io_observers_, DidProcessIOEvent());
}

bool MessagePumpLibevent2::Init() {
  int fds[2];

#if !defined(OS_WIN)
  if (pipe(fds)) {
    DLOG(ERROR) << "pipe() failed, errno: " << errno;
    return false;
  }
#else
if (SocketPairWin32(fds)) {
#if !defined(OS_WIN)
  DLOG(ERROR) << "pipe() failed, errno: " << FormatGetLastWSAErrorA(WSAGetLastError());
#else
  DLOG(ERROR) << "SetNonBlocking for pipe fd[0] failed, errno: " << FormatGetLastWSAErrorA(WSAGetLastError());
#endif
  return false;
}
#endif

  if (SetNonBlocking(fds[0])) {
#if !defined(OS_WIN)
    DLOG(ERROR) << "SetNonBlocking for pipe fd[0] failed, errno: " << errno;
#else
    DLOG(ERROR) << "SetNonBlocking for pipe fd[0] failed, errno: " << FormatGetLastWSAErrorA(WSAGetLastError());
#endif
    return false;
  }
  if (SetNonBlocking(fds[1])) {
#if !defined(OS_WIN)
    DLOG(ERROR) << "SetNonBlocking for pipe fd[1] failed, errno: " << errno;
#else
    DLOG(ERROR) << "SetNonBlocking for pipe fd[0] failed, errno: " << FormatGetLastWSAErrorA(WSAGetLastError());
#endif
    return false;
  }
  wakeup_pipe_out_ = fds[0];
  wakeup_pipe_in_ = fds[1];

  wakeup_event_ = new event;
  event_set(wakeup_event_, wakeup_pipe_out_, EV_READ | EV_PERSIST,
            OnWakeup, this);
  event_base_set(event_base_, wakeup_event_);

  if (event_add(wakeup_event_, 0))
    return false;
  return true;
}

// static
void MessagePumpLibevent2::OnLibeventNotification(int fd, short flags,
                                                 void* context) {
  base::WeakPtr<FileDescriptorWatcher> controller =
      static_cast<FileDescriptorWatcher*>(context)->weak_factory_.GetWeakPtr();
  DCHECK(controller.get());

  MessagePumpLibevent2* pump = controller->pump();
  pump->processed_io_events_ = true;

  if (flags & EV_WRITE) {
    controller->OnFileCanWriteWithoutBlocking(fd, pump);
  }
  // Check |controller| in case it's been deleted in
  // controller->OnFileCanWriteWithoutBlocking().
  if (controller.get() && flags & EV_READ) {
    controller->OnFileCanReadWithoutBlocking(fd, pump);
  }
}

// Called if a byte is received on the wakeup pipe.
// static
void MessagePumpLibevent2::OnWakeup(int socket, short flags, void* context) {
  base::MessagePumpLibevent2* that =
              static_cast<base::MessagePumpLibevent2*>(context);
  DCHECK(that->wakeup_pipe_out_ == socket);

  // Remove and discard the wakeup byte.
  char buf;
#if !defined(OS_WIN)
  int nread = HANDLE_EINTR(read(socket, &buf, 1));
#else
  int nread = HANDLE_EINTR(recv(socket, &buf, 1, 0));
#endif
  DCHECK_EQ(nread, 1);
  that->processed_io_events_ = true;
  // Tell libevent to break out of inner loop.
  event_base_loopbreak(that->event_base_);
}

}  // namespace base
