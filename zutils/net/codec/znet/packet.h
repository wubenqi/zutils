// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NETENGINE_ZNET_PACKET_H_
#define NETENGINE_ZNET_PACKET_H_

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
// #include "base2/byte_stream.h"

//接收处理数据包
class Packet :
	public base::RefCountedThreadSafe<Packet> {
public:
	static const uint32 kMaxPacketSize = 2*1024*1024; // 2MB
	static const uint32 kPacketHeaderSize = 8;
	static const uint32 kPacketVersion = 0;
	const static uint32 kPerPacketLen = 1024;

	// Packet();
	Packet(uint16 cmd_type, uint32 data_len);

	virtual ~Packet();

	inline int ParsePacketHeader( ) {
		uint16 cmdType;
		uint32 bodyLen;
		uint16 version;

		version = (uint16)head_[0]<<8 | head_[1];
		cmdType = (uint16)head_[2]<<8 | head_[3];
		bodyLen = (( head_[4] & 0xFF) << 24) | ((head_[5] & 0xFF) << 16) | ((head_[6] & 0xFF) <<  8) | (head_[7] & 0xFF);
		if (version!=kPacketVersion
			|| bodyLen > kMaxPacketSize ) {
			return -1;
		} else if (bodyLen ==0 ) {
			return 1;
		}
		return 0;
	}

	inline uint16 GetVersion() const {
		return (uint16)head_[0]<<8 | head_[1];
	}

	inline uint16 GetCmdType() const {
		return (uint16)head_[2]<<8 | head_[3];
	}

	inline uint32 GetBodyLength() const {
		return (( head_[4] & 0xFF) << 24) 
				| ((head_[5] & 0xFF) << 16) 
				| ((head_[6] & 0xFF) <<  8)
				| (head_[7] & 0xFF
			);
	}

	inline const void* GetBodyData() const { 
		return body_;
	}

	inline uint8* GetBodyData() { 
		return body_;
	}

	inline uint8* GetHeadMutableBuffer() {
		return head_;
	}

	inline uint8* GetBodyMutableData() {
		//uint32 len = GetBodyLength();
		//if (kPacketHeaderSize+kPerPacketLen < len) {
		//	ext_data_ = static_cast<uint8*>(malloc(len+kPacketHeaderSize));
		//	//LOG_ASSERT(ext_data_!=NULL);
		//	head_ = ext_data_;
		//	body_ = ext_data_+kPacketHeaderSize;
		//	head_[0] = (kPacketVersion >> 8) & 0xFF;
		//	head_[1] = kPacketVersion & 0xFF;
		//}
		return body_;
	}

	inline const uint8* GetRawdataConstBuffer() const {
		return head_;
	}

	inline uint32 GetRawdataLength() const {
		return GetBodyLength() + kPacketHeaderSize;
	}

	void Release2() {
		if(ext_data_) {
			free(ext_data_);
			ext_data_ = NULL;

			head_ = data_;
			body_ = data_+kPacketHeaderSize;
		}
	}

	Packet* Clone() const;

protected:
	// friend class OutPacketStream;
	//此构造函数只能在OutByteStream里使用,其它地方使用会出问题
	Packet(uint16 cmd_type, const void* data, uint32 data_len);
	Packet(const void* data, uint32 data_len);

	uint8*	head_;
	uint8*	body_;
	uint8*	data_;
	uint8*	ext_data_;

	DISALLOW_COPY_AND_ASSIGN(Packet);

};

typedef scoped_refptr<Packet> PacketPtr;

const PacketPtr& GetEmptyPacket();

#endif	//
