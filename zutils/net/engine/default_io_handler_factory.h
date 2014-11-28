// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_DEFAULT_IO_HANDLER_FACTORY_H_
#define NET_ENGINE_DEFAULT_IO_HANDLER_FACTORY_H_
#pragma once

#include "net/engine/io_handler_factory.h"

namespace net {

class DefaultIOHandlerFactory :
	public IOHandlerFactory {
public:
	static DefaultIOHandlerFactory* GetInstance() {
		return Singleton<DefaultIOHandlerFactory>::get();
	}

	virtual IOHandler* CreateIOHandler(SOCKET socket, Reactor* reactor/*, IOHandler::IOHandlerDelegate* ih_delegate*/) {
		return new IOHandler(socket, reactor/*, ih_delegate*/);
	}

private:
	friend class Singleton<DefaultIOHandlerFactory>;
	friend struct DefaultSingletonTraits<DefaultIOHandlerFactory>;

	DefaultIOHandlerFactory() {}
	virtual ~DefaultIOHandlerFactory() {}
};


}

#endif
