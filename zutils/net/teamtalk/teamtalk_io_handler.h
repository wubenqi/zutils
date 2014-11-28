// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_TEAMTALK_TEAMTALK_IO_HANDLER_H_
#define NET_TEAMTALK_TEAMTALK_IO_HANDLER_H_

#include "net/teamtalk/teamtalk_packet.h"
#include "net/engine/io_handler.h"
#include "net/base/data_io_buffer.h"
#include "base/memory/ref_counted.h"

namespace net {

class TeamTalkIOHandler :
	public IOHandler {
public:
	TeamTalkIOHandler(SOCKET socket, Reactor* reactor);
	virtual ~TeamTalkIOHandler();

	virtual int OnDataReceived(const TeamTalkPacketPtr& packet) { return 0; } 

	int SendData(const TeamTalkPacketPtr& packet);
	int SendSyncData(const TeamTalkPacketPtr& packet);

  void AsyncSendPacket(TeamTalkPacketPtr packet);

protected:
	virtual void Read();

private:
  void OnAsyncSendPacket(TeamTalkPacketPtr packet);
	int RecvData(uint8* data, int data_len);

	scoped_refptr<DataIOBuffer> read_buf_;
};

}

#endif	//
