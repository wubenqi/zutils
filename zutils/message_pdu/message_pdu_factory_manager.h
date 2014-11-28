// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef MESSAGE_PDU_MESSAGE_PDU_FACTORY_MANAGER_H_
#define MESSAGE_PDU_MESSAGE_PDU_FACTORY_MANAGER_H_

#include "base/memory/singleton.h"
#include "base/hash_tables.h"
#include "message_pdu/message_pdu_factory.h"

namespace message {

class MessagePDUFactoryManager;
typedef bool (*RegisterMessagePDUFactoryFunc)(MessagePDUFactoryManager*);

//单件
class MessagePDUFactoryManager {
public :
	static MessagePDUFactoryManager* GetInstance();

	//初始化接口
	// void Initialize();
	bool RegisterMessage(RegisterMessagePDUFactoryFunc func);

	//根据消息类型从内存里分配消息实体数据（允许多线程同时调用）
	MessagePDU* CreateMessage(MessagePDUType message_type);
	bool GetMessageNameAndNullable(MessagePDUType message_type, const char** message_name, bool* null_able);
	const char* GetMessageName(MessagePDUType message_type);

	//删除消息实体（允许多线程同时调用）
	// void RemoveMessage( const MessagePDU* message ) ;
	
	void AddFactory( MessagePDUFactory* pFactory ) ;

private:
	friend class Singleton<MessagePDUFactoryManager>;
	friend struct DefaultSingletonTraits<MessagePDUFactoryManager>;

  typedef base::hash_map<MessagePDUType, MessagePDUFactory*> MessageFactoryMap;

	MessagePDUFactoryManager();
	virtual ~MessagePDUFactoryManager() ;

  MessageFactoryMap factories_;
	// MessagePDUFactory**	factories_ ;
	// uint16 size_;
	// uint32* message_alloc_count_;
	// base::Lock lock_;
};


}

//每个应用程序都要实现这个函数。
bool RegisterPDUMessageFactory(message::MessagePDUFactoryManager* factory_manager);

//帮助宏
#define ADD_MESSAGEPDU_FACTORY(Message, MessageType) factory_manager->AddFactory(new message::MessagePDUFactoryTemplate<Message>(MessageType, #MessageType))

#endif
