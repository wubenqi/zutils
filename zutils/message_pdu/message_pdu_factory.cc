// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "message_pdu/message_pdu_factory.h"

namespace message {

const char kUnknownPDUMessage[] = "UNKNOWN PDU";

const char* MessagePDUFactory::GetUnknownMessageName() {
	return kUnknownPDUMessage;
}

}


