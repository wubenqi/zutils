// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef MESSAGE_PDU_MESSAGE_PUD_H_
#define MESSAGE_PDU_MESSAGE_PUD_H_

#include "base/basictypes.h"

typedef uint32 MessagePDUType;

namespace message {

class MessagePDU {
public:
	MessagePDU() {
		message_type_ = 0;
	}

	MessagePDU(MessagePDUType message_type) {
		message_type_ = message_type;
	}

	virtual ~MessagePDU() {
	}

	inline MessagePDUType message_type() { return message_type_; }
	inline const MessagePDUType message_type() const { return message_type_; }
	inline void set_message_type(MessagePDUType type) { message_type_ = type; }
	inline bool null_able() { return null_able_; }

	virtual uint32 ByteSize() const = 0;
	virtual bool ParseFromArray(const void* data, uint32 data_len) { return false; }
	virtual bool SerializeToArray(void* data, uint32 data_len) const { return false; }

private:
	MessagePDUType message_type_;
	bool null_able_;			// 消息体是否可以为空
};

}


#define CAST_PROTO_MESSAGE(MESSAGE, value) \
	const MESSAGE* value = reinterpret_cast<const MESSAGE*>(message); \
	if (!value) { \
		LOG(ERROR) << "recv a valid " << #MESSAGE << " data."; \
		return -1; \
	}

#endif
