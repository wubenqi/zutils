// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/codec/teamtalk/teamtalk_codec.h"

namespace net {

int TeamTalkCodec::OnDataReceived(const IOHandlerPtr& ih, IOBuffer* data, base::Time receive_time) {

  uint32 message_type;
  uint32 body_len;
  uint16 reserved;

  while (data->ReadableBytes()>=TeamTalkPacket::kPacketHeaderSize) {

    int ret = TeamTalkPacket::ParsePacketHeader((const uint8*)(data->Peek()),  &message_type, &reserved, &body_len);
    if (ret==-1) {
      return ret;
    } else if (ret==1) {
      TeamTalkPacketPtr p(new TeamTalkPacket(message_type, reserved, 0));
      int ret = delegate_->OnTTDataReceived(ih, p, receive_time);
      data->Retrieve(TeamTalkPacket::kPacketHeaderSize);
      return ret;
    } else if (ret==0) {
      if (data->ReadableBytes()>=(int)((body_len+TeamTalkPacket::kPacketHeaderSize))) {
        TeamTalkPacketPtr p(new TeamTalkPacket(message_type, reserved, body_len));
        memcpy(p->GetBodyMutableData(), data->Peek()+TeamTalkPacket::kPacketHeaderSize, body_len);
        int ret = delegate_->OnTTDataReceived(ih, p, receive_time);
        data->Retrieve(body_len+TeamTalkPacket::kPacketHeaderSize);
        return ret;
      } else {
        break;
      }
    }
  }

  return 0;
}

}
