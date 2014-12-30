// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/codec/znet/znet_codec.h"

#include "net/codec/znet/packet.h"
#include "net/codec/znet/znet_packet_header.h"

namespace net {

int ZNetCodec::OnDataReceived(const IOHandlerPtr& ih, IOBuffer* data, base::Time receive_time) {

  uint16 message_type;
  uint32 body_len;

  while (data->ReadableBytes()>=Packet::kPacketHeaderSize) {

    int ret = ZNetPacketHeader::ParsePacketHeader((const uint8*)(data->Peek()), &message_type, &body_len);
    if (ret==-1) {
      return ret;
    } else if (ret==1) {
      PacketPtr p(new Packet(message_type, 0));
      int ret = delegate_->OnZNetDataReceived(ih, p, receive_time);
      data->Retrieve(Packet::kPacketHeaderSize);
      return ret;
    } else if (ret==0) {
      if (data->ReadableBytes()>=(int)((body_len+Packet::kPacketHeaderSize))) {
        PacketPtr p(new Packet(message_type, body_len));
        memcpy(p->GetBodyMutableData(), data->Peek()+Packet::kPacketHeaderSize, body_len);
        int ret = delegate_->OnZNetDataReceived(ih, p, receive_time);
        data->Retrieve(body_len+Packet::kPacketHeaderSize);
        return ret;
      } else {
        break;
      }
    }
  }

  return 0;
}

}
