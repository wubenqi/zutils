// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_IO_HANDLER_FACTORY_H_
#define NET_ENGINE_IO_HANDLER_FACTORY_H_
#pragma once

#include "base/memory/singleton.h"
#include "net/engine/io_handler.h"

namespace net {

class IOHandlerFactory {
public:
	virtual IOHandler* CreateIOHandler(SOCKET socket, Reactor* reactor/*, IOHandler::IOHandlerDelegate* ih_delegate*/) = 0;

protected:
	IOHandlerFactory() {}
	virtual ~IOHandlerFactory ()  {}

	DISALLOW_COPY_AND_ASSIGN(IOHandlerFactory);
};

}

#endif
