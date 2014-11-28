// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/teamtalk/teamtalk_packet.h"

#include "base/logging.h"

const TeamTalkPacketPtr& TeamTalkPacket::GetEmptyPacket() {
	static TeamTalkPacketPtr kEmptyPacketPtr;
	return kEmptyPacketPtr;
}

TeamTalkPacket::TeamTalkPacket(uint32 message_type, uint32 body_len) {
	data_ = (uint8*)malloc(body_len+kPacketHeaderSize);
	CHECK(data_!=NULL);

  uint32 data_len = body_len+kPacketHeaderSize;
	data_[0] = (data_len >> 24) & 0xFF;
	data_[1] = (data_len >> 16) & 0xFF;
	data_[2] = (data_len >> 8) & 0xFF;
	data_[3] = data_len & 0xFF;

  data_[4] = (message_type >> 24) & 0xFF;
  data_[5] = (message_type >> 16) & 0xFF;
  data_[6] = (message_type >> 8) & 0xFF;
  data_[7] = message_type & 0xFF;

  data_[8] = (kPacketVersion >> 8) & 0xFF;
  data_[9] = kPacketVersion & 0xFF;

  data_[10] = (kReservedValue >> 8) & 0xFF;
  data_[11] = kReservedValue & 0xFF;
}

TeamTalkPacket::TeamTalkPacket(const void* data, uint32 data_len) {
	CHECK(data!=NULL);
  data_ = (uint8*)data;
}

TeamTalkPacket::~TeamTalkPacket() {
	free(data_);
}

TeamTalkPacket* TeamTalkPacket::Clone() const {
	int len = GetRawdataLength();
  //kPacketHeaderSize+GetBodyLength();
	uint8 *data = (uint8*)malloc(len);
	CHECK(data!=NULL);
	memcpy(data, data_, len);
	TeamTalkPacket* p=new TeamTalkPacket(data, len);
	CHECK(p!=NULL);
	return p;
}

