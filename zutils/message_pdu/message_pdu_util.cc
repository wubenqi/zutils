// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "message_pdu/message_pdu_util.h"

#include "base/logging.h"
//#include "base/memory/singleton.h"
#include "base/memory/scoped_ptr.h"

#include "message_pdu/message_pdu_factory_manager.h"
//#include "message_pdu/scoped_message_pdu.h"

namespace message {

MessagePDU* ParseMessagePDUFromNetPacker(const TeamTalkPacketPtr& packet) {

	MessagePDU* message = NULL;

	bool null_able = true;
	const char* message_name = NULL;
	uint32 message_type = packet->GetMessageType();

	do {
		if (!MessagePDUFactoryManager::GetInstance()->GetMessageNameAndNullable(message_type, &message_name, &null_able)) {
			LOG(ERROR) << "MessagePDUType[" 
				<< message_type 
				<< "] is "
				<< message_name;
			//肯定是攻击包
			//直接断开
			break;
		}

		LOG(INFO) << "MessagePDUType[" 
			<< message_type 
			<< "] is "
			<< message_name
			<< ", Nullable[" 
			<< null_able 
			<< "]";

		//判断是否属于正常消息
		message = MessagePDUFactoryManager::GetInstance()->CreateMessage(message_type);
		if (!null_able) {
			if(!message->ParseFromArray(packet->GetBodyData(), packet->GetBodyLength())) {

				// ScopedMessagePDU scoped_message(message);
				LOG(ERROR) << "Can not parse "
					<< message_name << " packet!";

        delete message;
				message = NULL;
				//数据包有误，请客户端检查发送格式
				break;
      }
      message->SetReserved(packet->GetReserved());
		}
	} while (0);

	return message;
}

MessagePDU* ParseMessagePDUFromArrayData(const char* data, uint32 data_len) {
  MessagePDU* message = NULL;

  bool null_able = true;
  const char* message_name = NULL;

  uint32 message_type = -1;
  uint32 body_len = 0;
  uint16 reserved = 0;
  
  // uint32 message_type = packet->GetMessageType();

  do {
    if (data == NULL || data_len <TeamTalkPacket::kPacketHeaderSize) {
      LOG(ERROR) << "data is null or data_len = " << data_len << ", <=kPacketHeaderSize[12]";
      break;
    }

    if (TeamTalkPacket::ParsePacketHeader(reinterpret_cast<const uint8*>(data), &message_type, &reserved, &body_len) == -1) {
      LOG(ERROR) << "ParsePacketHeader error, maybe attack data!!!!";
      break;;
    }

    if (!MessagePDUFactoryManager::GetInstance()->GetMessageNameAndNullable(message_type, &message_name, &null_able)) {
      LOG(ERROR) << "MessagePDUType[" 
        << message_type 
        << "] is "
        << message_name;
      //肯定是攻击包
      //直接断开
      break;
    }

    LOG(INFO) << "MessagePDUType[" 
      << message_type 
      << "] is "
      << message_name
      << ", Nullable[" 
      << null_able 
      << "]";

    //判断是否属于正常消息
    message = MessagePDUFactoryManager::GetInstance()->CreateMessage(message_type);
    if (!null_able) {
      if(!message->ParseFromArray(data+TeamTalkPacket::kPacketHeaderSize, body_len)) {

        // ScopedMessagePDU scoped_message(message);
        LOG(ERROR) << "Can not parse "
          << message_name << " packet!";

        delete message;
        message = NULL;
        //数据包有误，请客户端检查发送格式
        break;
      }
    }
    message->SetReserved(reserved);
  } while (0);

  return message;
}

TeamTalkPacketPtr MessageToPacket(const MessagePDU& message) {
  uint32 message_len = message.ByteSize();
  TeamTalkPacket* packet = new TeamTalkPacket(message.message_type(), message.GetReserved(), message_len);
  if (packet) {
    if(!message.SerializeToArray(packet->GetBodyMutableData(), message_len)) {
      const char* message_name = MessagePDUFactoryManager::GetInstance()->GetMessageName(message.message_type());
      LOG(ERROR) << "MessageType[" 
        << message_name 
        << "] SerializeToArray error.";
      delete packet;
      packet = NULL;
    }
  }
  return packet;
}

TeamTalkPacketPtr MessageToPacket(const MessagePDU* message) {
  DCHECK(message);
  return MessageToPacket(*message);
}

net::IOBufferPtr MessageToIOBuffer(const MessagePDU& message) {
  int message_size = message.ByteSize();
  net::IOBuffer* io_buffer = new net::IOBuffer(TeamTalkPacket::kPacketHeaderSize + message_size);
  TeamTalkPacket::GeneratePacketHeader(reinterpret_cast<uint8*>(io_buffer->data()), message.message_type(), message.GetReserved(), message_size);
  message.SerializeToArray(io_buffer->data()+TeamTalkPacket::kPacketHeaderSize, message_size);
  return io_buffer;
}

net::IOBufferPtr MessageToIOBuffer(const MessagePDU* message) {
  DCHECK(message);
  return MessageToIOBuffer(*message);
}

}
