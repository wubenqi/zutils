// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ZNET_ZNET_IO_HANDLER_FACTORY_H_
#define NET_ZNET_ZNET_IO_HANDLER_FACTORY_H_
#pragma once

#include "net/engine/io_handler_factory.h"
#include "net/znet/znet_io_handler.h"

namespace net {

class ZNetIOHandlerFactory :
	public IOHandlerFactory {
public:
	static ZNetIOHandlerFactory* GetInstance() {
		return Singleton<ZNetIOHandlerFactory>::get();
	}

	virtual IOHandler* CreateIOHandler(SOCKET socket, Reactor* reactor) {
		return new ZNetIOHandler(socket, reactor);
	}

private:
	friend class Singleton<ZNetIOHandlerFactory>;
	friend struct DefaultSingletonTraits<ZNetIOHandlerFactory>;

	ZNetIOHandlerFactory() {}
	virtual ~ZNetIOHandlerFactory() {}

	DISALLOW_COPY_AND_ASSIGN(ZNetIOHandlerFactory);
};

}

#endif	//
