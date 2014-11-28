// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_SESSION_BASE_SESSION_H_
#define NET_SESSION_BASE_SESSION_H_
#pragma once

#include "net/base/net_types.h"

namespace net {

class BaseSession {
public:
  enum SessionState {
    // ¿Í»§¶Ë³õÊ¼×´Ì¬
    kSessionState_Empty = 0,			//
    kSessionState_Connected = 1,			//
    kSessionState_Normal = 2,			//
    kSessionState_Closed = 3,			//
    kSessionState_Error = 4,			//
  };

  BaseSession(net::SessionID session_id)
    : session_id_(session_id)
    , state_(kSessionState_Empty) {
  }

  virtual ~BaseSession() {
  }

  virtual void OnNewSession() {
    state_ = kSessionState_Connected;
  }
  virtual int  OnSessionDataReceived(uint16 message_type, const void* data, uint32 data_len) {
    return 0;
  }
  virtual void OnSessionClosed() {
    state_ = kSessionState_Closed;
  }

  net::SessionID GetSessionID() {
    return session_id_;
  }

protected:
  net::SessionID session_id_;
  int state_;
};

}

#endif
