// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef AGENTD_CLIENT_COMM_HANDLER_H_
#define AGENTD_CLIENT_COMM_HANDLER_H_
#pragma once

#include "net/base/io_buffer.h"
#include "net/hammer/hammer_packet_header.h"
#include "net/engine/io_handler.h"

#include <google/protobuf/message.h>

inline net::IOBufferPtr MessageToIOBuffer(uint16 cmd_type, const google::protobuf::Message* message) {
  int message_size = message->ByteSize();
  net::IOBuffer* io_buffer = new net::IOBuffer(net::HammerPacketHeader::kPacketHeaderSize + message_size);
  net::HammerPacketHeader::GeneratePacketHeader(reinterpret_cast<uint8*>(io_buffer->data()), cmd_type, message_size);
  message->SerializeToArray(io_buffer->data()+net::HammerPacketHeader::kPacketHeaderSize, message_size);
  return io_buffer;
}

inline net::IOBufferPtr MessageToIOBuffer(uint16 cmd_type, const google::protobuf::Message& message) {
  int message_size = message.ByteSize();
  net::IOBuffer* io_buffer = new net::IOBuffer(net::HammerPacketHeader::kPacketHeaderSize + message_size);
  net::HammerPacketHeader::GeneratePacketHeader(reinterpret_cast<uint8*>(io_buffer->data()), cmd_type, message_size);
  message.SerializeToArray(io_buffer->data()+net::HammerPacketHeader::kPacketHeaderSize, message_size);
  return io_buffer;
}

inline void SendMessageData(net::IOHandler* ih, uint16 cmd_type, const google::protobuf::Message* message) {
  ih->AsyncSendIOBuffer(MessageToIOBuffer(cmd_type, message));
}

inline void SendMessageData(net::IOHandler* ih, uint16 cmd_type, const google::protobuf::Message& message) {
  ih->AsyncSendIOBuffer(MessageToIOBuffer(cmd_type, message));
}

#endif
