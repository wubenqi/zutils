// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NETENGINE_TEAMTALK_TEAMTALK_PACKET_H_
#define NETENGINE_TEAMTALK_TEAMTALK_PACKET_H_

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"

#include "net/base/byte_stream.h"

//接收处理数据包
class TeamTalkPacket :
	public base::RefCountedThreadSafe<TeamTalkPacket> {
public:
	static const uint32 kMaxPacketSize = 64*1024*1024; // 64MB
	static const uint32 kPacketHeaderSize = 12;
	static const uint16 kPacketVersion = 1;
  static const uint16 kReservedValue = 0;

	// Packet();
  // 注意：是body_len,packet_len = body_len+kPacketHeaderSize
	TeamTalkPacket(uint32 message_type, uint16 reserved, uint32 body_len);
	virtual ~TeamTalkPacket();

  static const scoped_refptr<TeamTalkPacket>& GetEmptyPacket();

  // 生成网络数据包头
  // 返回值：
  //  -1： 生成出错，数据包长度超过kMaxPacketSize
  //  0 ： 生成成功，有数据包体
  //  1 ： 生成成功，未带数据包体
  static inline int GeneratePacketHeader( uint8* data, uint32 message_type, uint16 reserved, uint32 body_len) {
    if (body_len>kMaxPacketSize) {
      return -1;
    }
    uint32 data_len = body_len+kPacketHeaderSize;
    data[0] = (data_len >> 24) & 0xFF;
    data[1] = (data_len >> 16) & 0xFF;
    data[2] = (data_len >> 8) & 0xFF;
    data[3] = data_len & 0xFF;

    data[4] = (message_type >> 24) & 0xFF;
    data[5] = (message_type >> 16) & 0xFF;
    data[6] = (message_type >> 8) & 0xFF;
    data[7] = message_type & 0xFF;

    data[8] = (kPacketVersion >> 8) & 0xFF;
    data[9] = kPacketVersion & 0xFF;

    data[10] = (reserved >> 8) & 0xFF;
    data[11] = reserved & 0xFF;

    return 0;
  }

	// 解析网络数据包头
	// 返回值：
	//  -1： 解析出错，如果版本号不对或者数据包长度超过kMaxPacketSize
	//  0 ： 解析成功，有数据包体
	//  1 ： 解析成功，未带数据包体
	static inline int ParsePacketHeader( const uint8* head_data, uint32* message_type, uint16* reserved, uint32* body_len ) {
		uint16 version;
    // uint16 reserved;
    uint32 packet_len;

    packet_len = (( head_data[0] & 0xFF) << 24) | ((head_data[1] & 0xFF) << 16) | ((head_data[2] & 0xFF) <<  8) | (head_data[3] & 0xFF);
    *message_type = (( head_data[4] & 0xFF) << 24) | ((head_data[5] & 0xFF) << 16) | ((head_data[6] & 0xFF) <<  8) | (head_data[7] & 0xFF);
    version = head_data[8] << 8 | head_data[9];
    *reserved = head_data[10] << 8 | head_data[11];

		if (version!=kPacketVersion ||
        packet_len > kMaxPacketSize ||
        packet_len < kPacketHeaderSize) {
				return -1;
		} else if (packet_len == kPacketHeaderSize ) {
      *body_len = 0;
			return 1;
		}
    *body_len = packet_len - kPacketHeaderSize;
		return 0;
	}

  inline uint32 GetRawdataLength() const {
    return (( data_[0] & 0xFF) << 24) |
        ((data_[1] & 0xFF) << 16) |
        ((data_[2] & 0xFF) <<  8) |
        (data_[3] & 0xFF
      );
  }

  inline const uint8* GetRawdataConstBuffer() const {
    return data_;
  }

  inline uint32 GetMessageType() const {
    return (( data_[4] & 0xFF) << 24) |
        ((data_[5] & 0xFF) << 16) |
        ((data_[6] & 0xFF) <<  8) |
        (data_[7] & 0xFF
      );
  }

  inline uint16 GetVersion() const {
    return (uint16)data_[8]<<8 | data_[9];
  }

  inline uint16 GetReserved() const {
    return (uint16)data_[10]<<8 | data_[11]; 
  }

  inline const void* GetBodyData() const { 
    return data_+kPacketHeaderSize;
	}

	inline uint8* GetBodyData() { 
    return data_+kPacketHeaderSize;
	}

  inline uint8* GetBodyMutableData() {
    return data_+kPacketHeaderSize;
  }

  inline uint32 GetBodyLength() const {
    return GetRawdataLength() - kPacketHeaderSize;
  }

	inline uint8* GetHeadMutableBuffer() {
		return data_;
	}

  inline uint32 GetHeaderLength() const {
    return kPacketHeaderSize;
  }

	TeamTalkPacket* Clone() const;

protected:
	// friend class OutPacketStream;
	// 此构造函数只能在OutByteStream里使用,其它地方使用会出问题
	// TeamTalkPacket(uint16 cmd_type, const void* data, uint32 data_len);
	TeamTalkPacket(const void* data, uint32 data_len);

	uint8*	data_;

	DISALLOW_COPY_AND_ASSIGN(TeamTalkPacket);

};

typedef scoped_refptr<TeamTalkPacket> TeamTalkPacketPtr;

#endif	//
