// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_CODEC_ZNET_ZNET_CODEC_H_
#define NET_ENGINE_CODEC_ZNET_ZNET_CODEC_H_

#include "net/engine/io_handler.h"
#include "net/codec/znet/packet.h"

namespace net {

class ZNetCodec : public IOHandler::Delegate {
public:
  class Delegate {
  public:
    virtual ~Delegate() {}

    virtual int  OnZNetNewConnection(const IOHandlerPtr& ih) { return 0; }
    virtual int  OnZNetDataReceived(const IOHandlerPtr& ih, const PacketPtr& packet, base::Time receive_time) { return 0; }
    virtual int  OnZNetConnectionClosed(const IOHandlerPtr& ih) { return 0; }
  };

  ZNetCodec(ZNetCodec::Delegate* delegate) 
    : delegate_(delegate) {
  }

  virtual ~ZNetCodec() {}

  virtual int  OnNewConnection(const IOHandlerPtr& ih) { 
    return delegate_->OnZNetNewConnection(ih);
  }

  virtual int  OnDataReceived(const IOHandlerPtr& ih, IOBuffer* data, base::Time receive_time);

  virtual int  OnConnectionClosed(const IOHandlerPtr& ih) {
    return delegate_->OnZNetConnectionClosed(ih);
  }

  bool SendPacket(const IOHandlerPtr& ih, const PacketPtr& packet) {
    return ih->SendData(packet->GetRawdataConstBuffer(), packet->GetRawdataLength());
  }

protected:
  ZNetCodec::Delegate* delegate_;
};

}

#endif
