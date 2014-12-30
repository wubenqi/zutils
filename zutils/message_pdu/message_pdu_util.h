// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef MESSAGE_PDU_MESSAGE_PDU_UTIL_H_
#define MESSAGE_PDU_MESSAGE_PDU_UTIL_H_
#pragma once

#include "net/codec/teamtalk/teamtalk_packet.h"

#include "message_pdu/message_pdu.h"

namespace message {

//收到网络数据包以后处理
//注意：
MessagePDU* ParseMessagePDUFromNetPacker(const TeamTalkPacketPtr& packet);
MessagePDU* ParseMessagePDUFromArrayData(const char* data, uint32 data_len);

TeamTalkPacketPtr MessageToPacket(const MessagePDU& message);
TeamTalkPacketPtr MessageToPacket(const MessagePDU* message);
//net::IOBufferPtr MessageToIOBuffer(const MessagePDU& message);
//net::IOBufferPtr MessageToIOBuffer(const MessagePDU* message);

}

#endif

