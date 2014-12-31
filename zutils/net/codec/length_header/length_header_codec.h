// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_CODEC_LENGTH_HEADER_LENGTH_HEADER_CODEC_H_
#define NET_ENGINE_CODEC_LENGTH_HEADER_LENGTH_HEADER_CODEC_H_

#include "net/engine/io_handler.h"
#include "net/codec/teamtalk/teamtalk_packet.h"

namespace net {

class LengthHeaderCodec : public IOHandler::Delegate {
public:
  class Delegate {
  public:
    virtual ~Delegate() {}

    virtual int OnLengthHeaderNewConnection(const IOHandlerPtr& ih) { return 0; }
    virtual int OnLengthHeaderDataReceived(const IOHandlerPtr& ih, const base::StringPiece& packet, base::Time receive_time) { return 0; }
    virtual int OnLengthHeaderConnectionClosed(const IOHandlerPtr& ih) { return 0; }
  };

  LengthHeaderCodec(LengthHeaderCodec::Delegate* delegate) 
    : delegate_(delegate) {
  }

  virtual ~LengthHeaderCodec() {}

  virtual int  OnNewConnection(const IOHandlerPtr& ih) { 
    return delegate_->OnLengthHeaderNewConnection(ih);
  }

  virtual int  OnDataReceived(const IOHandlerPtr& ih, IOBuffer* data, base::Time receive_time);

  virtual int  OnConnectionClosed(const IOHandlerPtr& ih) {
    return delegate_->OnLengthHeaderConnectionClosed(ih);
  }

  bool SendPacket(const IOHandlerPtr& ih, const TeamTalkPacketPtr& packet) {
    return ih->SendData(packet->GetRawdataConstBuffer(), packet->GetRawdataLength());
  }

protected:
  LengthHeaderCodec::Delegate* delegate_;

  const static size_t kHeaderLen = sizeof(int32);
};

}

#endif
