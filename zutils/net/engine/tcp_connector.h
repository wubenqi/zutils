// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_TCP_CONNECTOR_H_
#define NET_ENGINE_TCP_CONNECTOR_H_
#pragma once

#include "net/engine/socket_ops.h"
#include "base/message_loop.h"
#include "base/memory/ref_counted.h"

#include "net/base/io_buffer.h"
#include "net/engine/io_handler.h"

#include "net/engine/io_handler_factory.h"
#include "net/engine/io_handler.h"

namespace net {

class NetEngineManager;
class Reactor;

class TCPConnector :
	public base::RefCountedThreadSafe<TCPConnector>,
	public MessageLoopForIO::Watcher,
	public IOHandler::IOHandlerCallBack {
public:
  struct ConnAddrInfo {
    ConnAddrInfo()
      : is_inited(false)
    , is_numeric_host_address(true) {
    }

    void Destroy(){
      is_inited = false;
      ip.clear();
      port.clear();
      is_numeric_host_address = false;
      is_reconnect = false;
    }

    bool is_inited;
    std::string ip;
    std::string port;
    bool is_numeric_host_address;
    bool is_reconnect;
    int reconnect_time;
  };

	class TCPConnectorDelegate {
	public:
		virtual ~TCPConnectorDelegate() {}
		virtual void OnConnectedError(TCPConnector *conn, int err_code) {}
		virtual int OnNewConnection(TCPConnector *conn) { return 0; }
		virtual int OnConnectionClosed(TCPConnector *conn) { return 0; }
	};

	explicit TCPConnector(NetEngineManager* engine_manager, IOHandlerFactory* ih_factory = NULL, void* user_data = NULL, TCPConnectorDelegate* conn_delegate = NULL/*, IOHandler::IOHandlerDelegate* ih_delegate=NULL*/);
	virtual ~TCPConnector() {
	}

  // 同步发起连接请求
	bool Connect(const std::string& ip, const std::string& port, bool is_numeric_host_address, bool is_reconnect = false, int reconnect_time = 10);
  void DoReconnect(int reconnect_time);

	IOHandler* Attach(SOCKET connector);

	//bool Send(const char* data, int len);
  void AsyncSendIOBuffer(IOBufferPtr io_buffer);

	bool DisConnect();
	bool IsConnected();

	inline Reactor* GetReactor() { return reactor_; }
	inline IOHandler* GetIOHandler() { return io_handler_; }

protected:
  bool Connect(const ConnAddrInfo& conn_addr_info, bool is_sync);
  // 异步重连
  void ReConnect();

	// 实现 IOHandler::IOHandlerCallBack 接口
	virtual int OnNewConnection(IOHandler *ih);
	virtual int OnConnectionClosed(IOHandler *ih);
  void OnAsyncSendIOBuffer(IOBufferPtr io_buffer);

	// 
	void OnConnecting();
	void OnConnected();
	void OnDisConnect();

	WaitState wait_state_;
	// The socket's libevent wrapper
	MessageLoopForIO::FileDescriptorWatcher watcher_;
	// Called by MessagePumpLibevent2 when the socket is ready to do I/O
	virtual void OnFileCanReadWithoutBlocking(int fd);
	virtual void OnFileCanWriteWithoutBlocking(int fd);

private:
	// State machine for connecting the socket.
	enum ConnectState {
		CONNECT_STATE_CONNECT,
		CONNECT_STATE_CONNECT_COMPLETE,
		CONNECT_STATE_NONE,
	};

	// The next state for the Connect() state machine.
	ConnectState connect_state_;

	SOCKET connector_;
	Reactor* reactor_;
	NetEngineManager* engine_manager_;
	//IOHandler::IOHandlerDelegate* ih_delegate_;
	TCPConnectorDelegate* conn_delegate_;
	IOHandler* io_handler_;

	IOHandlerFactory* ih_factory_;

  ConnAddrInfo conn_addr_info_;
  void* user_data_;
};

typedef scoped_refptr<TCPConnector> TCPConnectorPtr;
}

#endif
