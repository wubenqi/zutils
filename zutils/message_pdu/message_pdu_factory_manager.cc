// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "message_pdu/message_pdu_factory_manager.h"

#include "base/logging.h"
#include "base/stl_util.h"

#include "base2/base_types.h"
#include "base2/map-util.h"
//#include "net/base/packet.h"


//#include "message/const_message_type.h"

namespace message {

MessagePDUFactoryManager::MessagePDUFactoryManager() {
// 	factories_ = NULL ;
// 	size_ = 0;
// 	message_alloc_count_ = NULL;
}

MessagePDUFactoryManager::~MessagePDUFactoryManager() {
  STLDeleteValues(&factories_);
}

MessagePDUFactoryManager* MessagePDUFactoryManager::GetInstance() {
	return Singleton<MessagePDUFactoryManager>::get();
}

// void MessagePDUFactoryManager::Initialize(uint16 max_message_size) {
// 	size_ = max_message_size ;
// 
// 	factories_ = new MessagePDUFactory*[size_];
// 	CHECK( factories_ ) ;
// 
// 	message_alloc_count_ = new uint32[size_] ;
// 	CHECK( message_alloc_count_ ) ;
// 
// 	for( uint16 i=0; i<size_; i++ ) {
// 		factories_[i] = NULL ;
// 		message_alloc_count_[i] = 0 ;
// 	}
// }

bool MessagePDUFactoryManager::RegisterMessage(RegisterMessagePDUFactoryFunc func) {

//  此处添加每种消息的Factory信息
//例如：
//	AddFactory( new CGAttackFactory() ) ;
//  .....
//CL && LC
	//AddFactory(new Message(PACKET_BL_RETAUTH, new hammer::message::BLRetAuth()));
	CHECK(func);
	if (func) {
		return func(this);
	}
	return false;
}

void MessagePDUFactoryManager::AddFactory(MessagePDUFactory* factory) {
  CHECK(factory);
  if (ContainsKey(factories_, factory->GetMessageType())) {
    LOG(FATAL) << "Message[" << factory->GetMessageName() << "] is in factories";
    return;
  }
	
  factories_.insert(std::make_pair(factory->GetMessageType(), factory));
}

MessagePDU* MessagePDUFactoryManager::CreateMessage(MessagePDUType message_type) {
  MessagePDU* message = NULL;

  MessagePDUFactory* message_pud_factory = base::FindPtrOrNull(factories_, message_type);
  if (message_pud_factory == NULL) {
    LOG(ERROR) << "MessageType = " << message_type << " not register factories";
  } else {
    message = message_pud_factory->CreateMessage();
  }
	
  return message;
}

bool MessagePDUFactoryManager::GetMessageNameAndNullable(MessagePDUType message_type, const char** message_name, bool* null_able) {
	CHECK(message_name);
	CHECK(null_able);

  bool result = true;

  MessagePDUFactory* message_pud_factory = base::FindPtrOrNull(factories_, message_type);
  if (message_pud_factory == NULL) {
    *message_name = MessagePDUFactory::GetUnknownMessageName();
    result = false;
  } else {
    *message_name = message_pud_factory->GetMessageName();
    *null_able = message_pud_factory->GetNullable();
  }
	
	return result;
}

const char* MessagePDUFactoryManager::GetMessageName(MessagePDUType message_type) {
  const char* message_name = MessagePDUFactory::GetUnknownMessageName();

  MessagePDUFactory* message_pud_factory = base::FindPtrOrNull(factories_, message_type);
	if( message_pud_factory ) {
    message_name = message_pud_factory->GetMessageName();
	} 

	return message_name;
}


// void MessagePDUFactoryManager::RemoveMessage(const MessagePDU* message) {
// 	if( message==NULL ) {
// 		LOG_ASSERT(FALSE) ;
// 		return ;
// 	}
// 
// 	MessagePDUType msessage_type = message->message_type();
// 	if( msessage_type>=size_ ) {
// 		LOG_ASSERT(FALSE) ;
// 		return ;
// 	}
// 
// 	lock_.Acquire();
// 
// 	SAFE_DELETE(message) ;
// 	message_alloc_count_[msessage_type]-- ;
// 
// 	lock_.Release();
// }


}
