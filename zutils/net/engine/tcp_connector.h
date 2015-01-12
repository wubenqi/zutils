// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_TCP_CONNECTOR_H_
#define NET_ENGINE_TCP_CONNECTOR_H_

#include "net/message_loop/message_loop_for_io2.h"
#include "net/engine/socket_ops.h"

#include "net/base/io_buffer.h"
#include "net/engine/io_handler.h"

namespace net {

// class NetEngineManager;

//线程安全
class TCPConnector: public base::MessageLoopForIO2::Watcher {
public:
  class Delegate {
  public:
    virtual bool OnCreateConnection(SOCKET s) { return false; }
    virtual bool OnDestroyConnection() { return false; }
  };

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
  explicit TCPConnector(base::MessageLoop* message_loop, Delegate* delegate);
	virtual ~TCPConnector() {
    DoClose(true);
	}

  // 同步发起连接请求
  // 线程安全
	bool Connect(const std::string& ip, const std::string& port, bool is_numeric_host_address);//, bool is_reconnect = false, int reconnect_time = 10);
  // bool IsConnected();
  bool DisConnect();

  // 非线程安全
  void DoClose(bool from_me);

protected:
  void DoConnect(int reconnect_time);
  void ReConnect();

	// 
	void OnConnecting();
	void OnConnected();
	void OnDisConnect();

  // 进行回调
  void OnNewConnection(SOCKET s);

	virtual void OnFileCanReadWithoutBlocking(int fd);
	virtual void OnFileCanWriteWithoutBlocking(int fd);

  WaitState wait_state_;
  // The socket's libevent wrapper
  base::MessageLoopForIO2::FileDescriptorWatcher watcher_;
  // Called by MessagePumpLibevent2 when the socket is ready to do I/O

private:
	// State machine for connecting the socket.
	enum ConnectState {
    kConnectStateInvalid = 0,       // 未连接
    kConnectStateConnecting,        // 正在连接
		kConnectStateConnectComplete,   // 连接成功，但未初始化
		kConnectStateConnected,         // 连接成功，可以开始收发数据
	};

  ConnectState GetConnectState() const {
    base::AutoLock lock(lock_);
    return connect_state_;
  }

  void SetConnState(ConnectState state) {
    base::AutoLock lock(lock_);
    connect_state_ = state;
  }

  bool IsConnectInvalidState() const {
    base::AutoLock lock(lock_);
    return connect_state_ == kConnectStateInvalid;
  }

  bool IsConnectedState() const {
    base::AutoLock lock(lock_);
    return connect_state_ == kConnectStateConnected;
  }

  bool IsConnectingState() const {
    base::AutoLock lock(lock_);
    return connect_state_ == kConnectStateConnecting;
  }

  base::MessageLoop* message_loop_;
  TCPConnector::Delegate* delegate_;

  ConnAddrInfo conn_addr_info_;

  mutable base::Lock lock_;
  // The next state for the Connect() state machine.
  ConnectState connect_state_;

  SOCKET connector_;

};

}

#endif
