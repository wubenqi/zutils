// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_CODEC_TEAMTALK_TEAMTALK_CODEC_H_
#define NET_ENGINE_CODEC_TEAMTALK_TEAMTALK_CODEC_H_

#include "net/engine/io_handler.h"
#include "net/codec/teamtalk/teamtalk_packet.h"

namespace net {

class TeamTalkCodec : public IOHandler::Delegate {
public:
  class Delegate {
  public:
    virtual ~Delegate() {}

    virtual int OnTTNewConnection(const IOHandlerPtr& ih) { return 0; }
    virtual int OnTTDataReceived(const IOHandlerPtr& ih, const TeamTalkPacketPtr& packet, base::Time receive_time) { return 0; }
    virtual int OnTTConnectionClosed(const IOHandlerPtr& ih) { return 0; }
  };

  TeamTalkCodec(TeamTalkCodec::Delegate* delegate) 
    : delegate_(delegate) {
  }

  virtual ~TeamTalkCodec() {}

  virtual int  OnNewConnection(const IOHandlerPtr& ih) { 
    return delegate_->OnTTNewConnection(ih);
  }

  virtual int  OnDataReceived(const IOHandlerPtr& ih, IOBuffer* data, base::Time receive_time);

  virtual int  OnConnectionClosed(const IOHandlerPtr& ih) {
    return delegate_->OnTTConnectionClosed(ih);
  }

  bool SendPacket(const IOHandlerPtr& ih, const TeamTalkPacketPtr& packet) {
    return ih->SendData(packet->GetRawdataConstBuffer(), packet->GetRawdataLength());
  }

protected:
  TeamTalkCodec::Delegate* delegate_;
};

}

#endif
