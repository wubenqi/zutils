// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/codec/znet/packet.h"

#include "base/logging.h"

const PacketPtr& GetEmptyPacket() {
	static PacketPtr kEmptyPacketPtr;
	return kEmptyPacketPtr;
}

//Packet::Packet() {
//	data_ = (uint8*)malloc(kPacketHeaderSize);
//	CHECK(data_!=NULL);
//
//	head_ = data_;
//	body_ = data_+kPacketHeaderSize;
//	ext_data_ = NULL;
//}

Packet::Packet(uint16 cmd_type, uint32 data_len) {
	ext_data_ = NULL;
	data_ = (uint8*)malloc(data_len+kPacketHeaderSize);
	CHECK(data_!=NULL);
	head_ = data_;
	body_ = data_+kPacketHeaderSize;
	//m_maxLen = maxLen;

	head_[0] = (kPacketVersion >> 8) & 0xFF;
	head_[1] = kPacketVersion & 0xFF;
	head_[2] = (cmd_type >> 8) & 0xFF;
	head_[3] = cmd_type & 0xFF;
	head_[4] = (data_len >> 24) & 0xFF;
	head_[5] = (data_len >> 16) & 0xFF;
	head_[6] = (data_len >> 8) & 0xFF;
	head_[7] = data_len & 0xFF;
}

Packet::Packet(uint16 cmd_type, const void* data, uint32 data_len) {
	ext_data_ = NULL;
	data_ = (uint8*)data;
	CHECK(data_!=NULL);
	head_ = data_;
	body_ = data_+kPacketHeaderSize;
	//m_maxLen = maxLen;

	head_[0] = (kPacketVersion >> 8) & 0xFF;
	head_[1] = kPacketVersion & 0xFF;
	head_[2] = (cmd_type >> 8) & 0xFF;
	head_[3] = cmd_type & 0xFF;
	head_[4] = (data_len >> 24) & 0xFF;
	head_[5] = (data_len >> 16) & 0xFF;
	head_[6] = (data_len >> 8) & 0xFF;
	head_[7] = data_len & 0xFF;
}

Packet::Packet(const void* data, uint32 data_len) {
	CHECK(data!=NULL);
	ext_data_ = NULL;
	data_ = (uint8*)data;
	head_ = data_;
	body_ = data_+kPacketHeaderSize;
}

Packet::~Packet() {
	Release2();
	free(data_);
}

Packet* Packet::Clone() const {
	int len = kPacketHeaderSize+GetBodyLength();
	uint8 *data = (uint8*)malloc(len);
	CHECK(data!=NULL);
	memcpy(data, head_, len);
	Packet* p=new Packet(data, len);
	CHECK(p!=NULL);
	return p;
}

