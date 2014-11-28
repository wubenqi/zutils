// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_REACTOR_H_
#define NET_ENGINE_REACTOR_H_
#pragma once

#include "base/id_map.h"
#include "base/threading/thread.h"
#include "net/base/io_buffer.h"
#include "base/timer.h"

namespace net {
class IOHandler;
class NetEngineManagerDelegate;
class Reactor :
	public base::Thread {
public:
  explicit Reactor(const std::string& name, int reactor_id, NetEngineManagerDelegate* net_engine_delegate=NULL);
	~Reactor();

	bool Run();
	void Shutdown();

  void DoSendSessionData(uint32 io_handler_id, IOBufferPtr io_buffer);
  void DoPostTaskData(uint32 io_handler_id, IOBufferPtr io_buffer);

  inline int GetReactorID() {
    return reactor_id_;
  }

  inline int32 AddIOHandler(IOHandler* ih) {
    return io_handlers_.Add(ih);
  }
  inline void Remove(int32 session_id) {
    io_handlers_.Remove(session_id);
  }
  inline IDMap<IOHandler>& GetIOHandlers() {
    return io_handlers_;
  }

  inline base::Timer* GetTimer() {
    return timer_;
  }

  void AttachUserData(void* user_data) {
    user_data_ = user_data;
  }
  void* DetachUserData() {
    void* user_data = user_data_;
    user_data_ = NULL;
    return user_data;
  }
  void* GetUserData() {
    return user_data_;
  }

protected:
  // Called just prior to starting the message loop
  virtual void Init();
  // Called just after the message loop ends
  virtual void CleanUp();

private:
  void OnSendSessionData(uint32 io_handler_id, IOBufferPtr io_buffer);
  void OnTaskData(uint32 io_handler_id, IOBufferPtr io_buffer);

  int reactor_id_;
  IDMap<IOHandler> io_handlers_;
  base::Timer* timer_;
  NetEngineManagerDelegate* net_engine_delegate_;
  void* user_data_;
};

}

#endif
