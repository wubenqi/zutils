// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef MESSAGE_PDU_MESSAGE_PDU_FACTORY_H_
#define MESSAGE_PDU_MESSAGE_PDU_FACTORY_H_

#include "base/basictypes.h"
#include "message_pdu/message_pdu.h"

namespace message {

class MessagePDUFactory {
public:
	MessagePDUFactory() {}
	virtual ~MessagePDUFactory ()  {}

	virtual MessagePDU* CreateMessage()  = 0;
	virtual MessagePDUType GetMessageType() const = 0;
	virtual bool GetNullable() const = 0;
	virtual const char* GetMessageName() const = 0;

	//////////////////////////////////////////////////////////////////////////
	static const char* GetUnknownMessageName();
protected:
	DISALLOW_COPY_AND_ASSIGN(MessagePDUFactory);
};


//使用法
template <class Message2>
class MessagePDUFactoryTemplate :
	public MessagePDUFactory {
public:
	MessagePDUFactoryTemplate(MessagePDUType message_type, const char* message_name, bool null_able = false) { 
		message_type_ = message_type;
		message_name_ = message_name ? message_name : MessagePDUFactory::GetUnknownMessageName();
		null_able_ = null_able;
	}

	virtual ~MessagePDUFactoryTemplate ()  {}

	virtual MessagePDU* CreateMessage() {
		if (null_able_) {
			//return new Message(message_type_, NULL);
			return NULL;
		} else {
			return new Message2();
			//return new Message(message_type_, new Message2());
		}
	}

	virtual MessagePDUType GetMessageType() const {
		return message_type_;
	}

	virtual const char* GetMessageName() const {
		return message_name_;
	}

	virtual bool GetNullable() const {
		return null_able_;
	}


private:
	MessagePDUType message_type_;		// 消息类型
	bool null_able_;			// 消息体是否可以为空
	const char* message_name_;	//
	DISALLOW_COPY_AND_ASSIGN(MessagePDUFactoryTemplate);
};

}

#endif
