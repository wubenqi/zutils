// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/znet/znet_io_handler.h"

#include "base/eintr_wrapper.h"
#include "base/bind.h"
#include "base/message_loop.h"

#include "net/engine/reactor.h"
#include "net/znet/znet_packet_header.h"

#if defined(OS_WIN)
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>
#include <setjmp.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include<linux/tcp.h>
#endif

namespace {
const int kReadBufSize = 4096;
}

namespace net {

ZNetIOHandler::ZNetIOHandler(SOCKET socket, Reactor* reactor)
	: IOHandler(socket, reactor)
/*
	, data_(new Packet())
	, body_(NULL)
	, cur_body_pos_(0)
	, cur_head_pos_(0)
	, is_header_(true)
	, is_ended_(false) {
*/
	, read_buf_(new DataIOBuffer(kReadBufSize)) {
}

ZNetIOHandler::~ZNetIOHandler() {

}

void ZNetIOHandler::Read() {
	uint8 buf[kReadBufSize];  // +1 for null termination
	int read_len = RecvData(buf, kReadBufSize);
	// LOG(INFO) << "recv data, data_len = " << read_len;
	if (read_len>0) {
		read_buf_->append((const char*)buf, read_len);
		uint16 message_type;
		uint32 body_len;

		while (read_buf_->read_data_size()>=Packet::kPacketHeaderSize) {

			int ret = ZNetPacketHeader::ParsePacketHeader((const uint8*)(read_buf_->read_data()), &message_type, &body_len);
			if (ret==-1) {
				Close();
				LOG(ERROR) << "Recv a valid packet";
				return;
			} else if (ret==1) {
				PacketPtr data(new Packet(message_type, 0));
				int ret = OnDataReceived(data);
				read_buf_->skip_read_pos(Packet::kPacketHeaderSize);
				read_buf_->remove_read_data();
				if (ret==-1) {
					Close();
					return;
				}
			} else if (ret==0) {
				if (read_buf_->read_data_size()>=(int)((body_len+Packet::kPacketHeaderSize))) {
					PacketPtr data(new Packet(message_type, body_len));
					memcpy(data->GetBodyMutableData(), read_buf_->read_data()+Packet::kPacketHeaderSize, body_len);
					int ret = OnDataReceived(data);
					read_buf_->skip_read_pos(body_len+Packet::kPacketHeaderSize);
					read_buf_->remove_read_data();
					if (ret==-1) {
						Close();
						return;
					}
				} else {
					break;
				}
			}
		}
	}

/*

	if (-1 == RecvData(data_->GetHeadMutableBuffer(), Packet::kPacketHeaderSize) ) {
		return;
	}

	int ret = data_->ParsePacketHeader();
	if (1==ret) {
		OnDataReceived(data_);
	} else if (0==ret) {
		if(-1==RecvData(data_->GetBodyMutableData(), data_->GetBodyLength())) {
			return;
		}
		OnDataReceived(data_);
	} else {

	}
*/

}

int ZNetIOHandler::RecvData(uint8* data, int data_len) {
	int len = data_len;
	int read_len  = 0;
	do {
		len = HANDLE_EINTR(recv(socket_, (char *)(data+read_len), data_len-read_len, 0));
		if (len == kSocketError) {
#if defined(OS_WIN)
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK) {
#elif defined(OS_POSIX)
			if (errno == EWOULDBLOCK || errno == EAGAIN) {
#endif
				//Close();
				//return -1;
				break;
			} else {
				// TODO(ibrar): some error handling required here
				Close();
				return -1;
				// break;
			}
		} else if (len == 0) {
			// In Windows, Close() is called by OnObjectSignaled.  In POSIX, we need
			// to call it here.
			 Close();
			 return -1;
			break;
		} else {
			// TODO(ibrar): maybe change DidRead to take a length instead
			DCHECK(len > 0 && len <= data_len);
			read_len += len;
		}
	} while (read_len < data_len);
	return read_len;
}

int ZNetIOHandler::SendData(const PacketPtr& packet) {
	AsyncSend(reinterpret_cast<const char*>(packet->GetRawdataConstBuffer()), packet->GetRawdataLength());
	return 0;
}

int ZNetIOHandler::SendSyncData(const PacketPtr& packet) {
	SendInternal(reinterpret_cast<const char*>(packet->GetRawdataConstBuffer()), packet->GetRawdataLength());
	return 0;
}

void ZNetIOHandler::AsyncSendPacket(PacketPtr packet) {
  GetReactor()->message_loop()->PostTask(FROM_HERE, base::Bind(&ZNetIOHandler::OnAsyncSendPacket, base::Unretained(this), packet));
}

void ZNetIOHandler::OnAsyncSendPacket(PacketPtr packet) {
  SendInternal(reinterpret_cast<const char*>(packet->GetRawdataConstBuffer()), packet->GetRawdataLength());
}


}
