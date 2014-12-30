// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_REACTOR_THREAD_H_
#define NET_ENGINE_REACTOR_THREAD_H_

#include "base/memory/scoped_ptr.h"
#include "base/threading/thread.h"

namespace net {

// message_loop的生命周期自己管理，自带线程
class ReactorThread : public base::Thread {
public:
  class Delegate {
  public:
    virtual ~Delegate() {}
    virtual void OnReactorCreate(ReactorThread* reactor) {}
    virtual void OnReactorDestroy(ReactorThread* reactor) {}
  };

  explicit ReactorThread(int reactor_id, Delegate* delegate = NULL);
	virtual ~ReactorThread();

	bool Start();
	void Shutdown();

  inline int GetReactorID() {
    return reactor_id_;
  }

protected:
  // Called just prior to starting the message loop
  virtual void Init();
  // Called just after the message loop ends
  virtual void CleanUp();

private:
  int reactor_id_;
  Delegate* delegate_;

  DISALLOW_COPY_AND_ASSIGN(ReactorThread);
};

}

#endif
