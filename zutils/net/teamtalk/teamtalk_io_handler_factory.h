// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_TEAMTALK_TEAMTALK_IO_HANDLER_FACTORY_H_
#define NET_TEAMTALK_TEAMTALK_IO_HANDLER_FACTORY_H_
#pragma once

#include "net/engine/io_handler_factory.h"
#include "net/teamtalk/teamtalk_io_handler.h"

namespace net {

class TeamTalkIOHandlerFactory :
	public IOHandlerFactory {
public:
	static TeamTalkIOHandlerFactory* GetInstance() {
		return Singleton<TeamTalkIOHandlerFactory>::get();
	}

	virtual IOHandler* CreateIOHandler(SOCKET socket, Reactor* reactor) {
		return new TeamTalkIOHandler(socket, reactor);
	}

private:
	friend class Singleton<TeamTalkIOHandlerFactory>;
	friend struct DefaultSingletonTraits<TeamTalkIOHandlerFactory>;

	TeamTalkIOHandlerFactory() {}
	virtual ~TeamTalkIOHandlerFactory() {}

	DISALLOW_COPY_AND_ASSIGN(TeamTalkIOHandlerFactory);
};

}

#endif	//
