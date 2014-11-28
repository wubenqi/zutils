// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/engine/tcp_connector.h"

#include "base/message_loop.h"
#include "base/bind.h"

#include "net/base/net_errors.h"
#include "net/engine/reactor.h"
#include "net/engine/net_engine_manager.h"

#include "net/engine/default_io_handler_factory.h"

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

namespace net {

namespace {
#if defined(OS_WIN)
int MapConnectError(int os_error) {
switch (os_error) {
	// connect fails with WSAEACCES when Windows Firewall blocks the
	// connection.
	case WSAEACCES:
		return ERR_NETWORK_ACCESS_DENIED;
	case WSAETIMEDOUT:
		return ERR_CONNECTION_TIMED_OUT;
	default: {
		int net_error = MapSystemError(os_error);
		if (net_error == ERR_FAILED)
			return ERR_CONNECTION_FAILED;  // More specific than ERR_FAILED.

/*
		// Give a more specific error when the user is offline.
		if (net_error == ERR_ADDRESS_UNREACHABLE &&
			NetworkChangeNotifier::IsOffline()) {
				return ERR_INTERNET_DISCONNECTED;
		}
*/
		return net_error;
	 }
}
}
#else
int MapConnectError(int os_error) {
	switch (os_error) {
		case EACCES:
			return ERR_NETWORK_ACCESS_DENIED;
		case ETIMEDOUT:
			return ERR_CONNECTION_TIMED_OUT;
		default: {
			int net_error = MapSystemError(os_error);
			if (net_error == ERR_FAILED)
				return ERR_CONNECTION_FAILED;  // More specific than ERR_FAILED.

/*
			// Give a more specific error when the user is offline.
			if (net_error == ERR_ADDRESS_UNREACHABLE &&
				NetworkChangeNotifier::IsOffline()) {
					return ERR_INTERNET_DISCONNECTED;
			}
*/
			return net_error;
		 }
	}
}

#endif
}


TCPConnector::TCPConnector(NetEngineManager* engine_manager, IOHandlerFactory* ih_factory, void* user_data, TCPConnectorDelegate* conn_delegate/*, IOHandler::IOHandlerDelegate* ih_delegate*/)	
	: engine_manager_(engine_manager)
	, ih_factory_(ih_factory)
	, conn_delegate_(conn_delegate)
	//, ih_delegate_(ih_delegate)
	, connector_(kInvalidSocket)
	, connect_state_(CONNECT_STATE_NONE)
	, io_handler_(NULL)
  , user_data_(user_data) {
	
	DCHECK(engine_manager);
	if (ih_factory==NULL) {
		//ih_factory_ = DefaultIOHandlerFactory::GetInstance();
	}
	
	reactor_ = engine_manager->GetReactor();
}

bool TCPConnector::Connect(const ConnAddrInfo& conn_addr_info, bool is_sync) {
  // 状态不合法
  if(connect_state_!=CONNECT_STATE_NONE) {
    return false;
  }

  if (is_sync) {
    // 同步连接
    if(CreateBlockingTCPConnectedSocket(conn_addr_info.ip, conn_addr_info.port, conn_addr_info.is_numeric_host_address, &connector_)) {
      //connect_state_ = CONNECT_STATE_CONNECT_COMPLETE;
      scoped_refptr<IOHandler> ih = ih_factory_->CreateIOHandler(connector_, reactor_/*, ih_delegate_*/);
      ih->SetUserData(user_data_);
      //ih->AddIOHandlerDelegate(this);
      ih->Create();
      return true;
    } else {
      //if (conn_addr_info.is_reconnect) {
      //  reactor_->message_loop()->PostTask(FROM_HERE, base::Bind(&TCPConnector::OnConnecting, base::Unretained(this)));

      //  //GetReactor()->GetTimer()->Start(FROM_HERE, base::TimeDelta::FromSeconds(conn_addr_info.reconnect_time), base::Bind(&TCPConnector::ReConnect, base::Unretained(this)));
      //  return true;
      //}
      return false;
    }
  } else {
    // 异步连接
    int ret = CreateTCPConnectedSocket(conn_addr_info.ip, conn_addr_info.port, conn_addr_info.is_numeric_host_address, &connector_);
    if ( ret == -1 ) {
      LOG(ERROR) << "Unable to connect host for: ret = " << ret
        << ": " << conn_addr_info.ip << ":"
        << conn_addr_info.port;
      return false;
    }

    if (ret==1) {
      //connect_state_ = CONNECT_STATE_CONNECT_COMPLETE;
      scoped_refptr<IOHandler> ih = ih_factory_->CreateIOHandler(connector_, reactor_/*, ih_delegate_*/);
      ih->SetUserData(user_data_);
      //ih->AddIOHandlerDelegate(this);
      ih->Create();
      return true;
    }

    connect_state_ = CONNECT_STATE_CONNECT;
    reactor_->message_loop()->PostTask(FROM_HERE, base::Bind(&TCPConnector::OnConnecting, base::Unretained(this)));
  }

  return true;
}

bool TCPConnector::Connect(const std::string& ip, const std::string& port, bool is_numeric_host_address, bool is_reconnect, int reconnect_time) {
  // 缓存ConnAddrInfo,为了断线重连

  conn_addr_info_.is_inited = true;
  conn_addr_info_.ip = ip;
  conn_addr_info_.port = port;
  conn_addr_info_.is_numeric_host_address = is_numeric_host_address;
  conn_addr_info_.is_reconnect = is_reconnect;
  conn_addr_info_.reconnect_time = reconnect_time;

  bool result = Connect(conn_addr_info_, false);
  if (result == false) {
    reactor_->message_loop()->PostTask(FROM_HERE, base::Bind(&TCPConnector::DoReconnect, base::Unretained(this), reconnect_time));
  }
  
  return result;
}

void TCPConnector::DoReconnect(int reconnect_time) {
  GetReactor()->GetTimer()->Start(FROM_HERE, base::TimeDelta::FromSeconds(reconnect_time), base::Bind(&TCPConnector::ReConnect, base::Unretained(this)));
}

void TCPConnector::ReConnect() {
  if (conn_addr_info_.is_inited) {
    Connect(conn_addr_info_, false);
  }
}

IOHandler* TCPConnector::Attach(SOCKET connector) {
	if(connect_state_!=CONNECT_STATE_NONE || connector_!=kInvalidSocket) {
		return NULL;
	}
	connector_ = connector;
	scoped_refptr<IOHandler> ih = ih_factory_->CreateIOHandler(connector_, reactor_/*, ih_delegate_*/);
	ih->SetIOHandlerCallBack(this);
  ih->SetUserData(user_data_);
	//ih->AddIOHandlerDelegate(this);
	ih->Create();
	// io_handler_ = ih.get();
	// connect_state_ = CONNECT_STATE_CONNECT_COMPLETE;
	return ih.get();
}

//bool TCPConnector::Send(const char* data, int len) {
//	if (!IsConnected()) {
//		return false;
//	}
//	io_handler_->AsyncSend(data, len);
//	return true;
//}

void TCPConnector::AsyncSendIOBuffer(IOBufferPtr io_buffer) {
  GetReactor()->message_loop()->PostTask(FROM_HERE, base::Bind(&TCPConnector::OnAsyncSendIOBuffer, base::Unretained(this), io_buffer));
}

void TCPConnector::OnAsyncSendIOBuffer(IOBufferPtr io_buffer) {
	if (IsConnected()) {
    io_handler_->AsyncSendIOBuffer(io_buffer);
	}
}

bool TCPConnector::IsConnected() { 
	return io_handler_!=NULL && connect_state_==CONNECT_STATE_CONNECT_COMPLETE;
}

int TCPConnector::OnNewConnection(IOHandler *ih) {
	int ret = 0;
	io_handler_ = ih;
	connect_state_ = CONNECT_STATE_CONNECT_COMPLETE;

  if (conn_delegate_) {
    conn_delegate_->OnNewConnection(this);
  }
	return ret;
}

/*
int TCPConnector::OnDataReceived(IOHandler *ih, const char* data, int len) {
	int ret = 0;
	//if (ih_delegate_) {
	//	ret = ih_delegate_->OnDataReceived(ih, data, len);
	//}
	return ret;
}
*/

int TCPConnector::OnConnectionClosed(IOHandler *ih) {
  if (conn_delegate_) {
    conn_delegate_->OnConnectionClosed(this);
  }

	int ret = 0;
	io_handler_ = NULL;
  connector_ = kInvalidSocket;
	connect_state_ = CONNECT_STATE_NONE;

  if (conn_addr_info_.is_reconnect) {
    DoReconnect(conn_addr_info_.reconnect_time);
  }
  
	return ret;
}

void TCPConnector::OnDisConnect() {
	if (io_handler_) {
		IOHandler* ih = io_handler_;
		ih->Close();
	} else {
		if (connector_ != kInvalidSocket) {
#if defined(OS_WIN)
			// Note: don't use CancelIo to cancel pending IO because it doesn't work
			// when there is a Winsock layered service provider.

			// In most socket implementations, closing a socket results in a graceful
			// connection shutdown, but in Winsock we have to call shutdown explicitly.
			// See the MSDN page "Graceful Shutdown, Linger Options, and Socket Closure"
			// at http://msdn.microsoft.com/en-us/library/ms738547.aspx
			shutdown(connector_, SD_SEND);
			closesocket(connector_);
#elif defined(OS_POSIX)
			close(connector_);
#endif
			connector_ = kInvalidSocket;
			connect_state_ = CONNECT_STATE_NONE;
		}
	}
}

bool TCPConnector::DisConnect() {
	if (connect_state_!=CONNECT_STATE_CONNECT_COMPLETE || connector_==kInvalidSocket) {
		return false;
	}
	
	//reactor_->message_loop()->PostTask(FROM_HERE, NewRunnableMethod(
	//	this, &TCPConnector::OnDisConnect));

 reactor_->message_loop()->PostTask(FROM_HERE, base::Bind(&TCPConnector::OnDisConnect, this));

	return true;
}

void TCPConnector::OnConnecting() {
	// Otherwise the connect() is going to complete asynchronously, so watch
	// for its completion.

#if defined(OS_WIN)
	if (!MessageLoopForIO::current()->WatchFileDescriptor(
			connector_, true, MessageLoopForIO::WATCH_READ_WRITE, &watcher_, this)) {
		// 出错如何处理？？
		int os_error = 0;
		os_error = WSAGetLastError();
		LOG(ERROR) << "WatchFileDescriptor failed: " << FormatLastWSAErrorA(os_error);
	}
#else
	if (!MessageLoopForIO::current()->WatchFileDescriptor(
			connector_, true, MessageLoopForIO::WATCH_WRITE, &watcher_, this)) {
		// 出错如何处理？？
		int os_error = errno;
		LOG(ERROR) << "WatchFileDescriptor failed: " << strerror(os_error);
	}

#endif

}

void TCPConnector::OnConnected() {

	if (connect_state_ != CONNECT_STATE_CONNECT) {
		LOG(ERROR) << "state error";
		return;
	}

	// Get the error that connect() completed with.
	int os_error = 0;
	socklen_t len = sizeof(os_error);
	if (getsockopt(connector_, SOL_SOCKET, SO_ERROR, (char*)&os_error, &len) < 0) {
#if defined(OS_WIN)
		os_error = WSAGetLastError();
#else
		os_error = errno;
#endif
	}

#if defined(OS_WIN)
	// TODO(eroman): Is this check really necessary?
	if (os_error == WSAEINPROGRESS || os_error == WSAEALREADY) {
		NOTREACHED();  // This indicates a bug in libevent or our code.
		return;
	}
#else
	// TODO(eroman): Is this check really necessary?
	if (os_error == EINPROGRESS || os_error == EALREADY) {
		NOTREACHED();  // This indicates a bug in libevent or our code.
		return;
	}
#endif

	watcher_.StopWatchingFileDescriptor();

	int result = MapConnectError(os_error);
	if (result == OK) {
		scoped_refptr<IOHandler> ih = ih_factory_->CreateIOHandler(connector_, reactor_/*, ih_delegate_*/);
    ih->SetUserData(user_data_);
		ih->SetIOHandlerCallBack(this);
		//ih->AddIOHandlerDelegate(this);
		ih->Create();

	} else {

#if defined(OS_WIN)
		LOG(ERROR) << "Connect host failed: " << FormatLastWSAErrorA(os_error);
#else
		LOG(ERROR) << "Connect host failed: " << strerror( os_error );
#endif

		if (conn_delegate_) {
			conn_delegate_->OnConnectedError(this, result);
		}

    if (conn_addr_info_.is_reconnect) {
      DoReconnect(conn_addr_info_.reconnect_time);
    }

		OnDisConnect();
	}
}

void TCPConnector::OnFileCanReadWithoutBlocking(int fd) {
	// MessagePumpLibevent callback, we don't listen for write events
	// so we shouldn't ever reach here.
#if defined(OS_WIN)
	if (connect_state_ == CONNECT_STATE_CONNECT) {
		OnConnected();
	} 
#else
	NOTREACHED();
#endif
}

void TCPConnector::OnFileCanWriteWithoutBlocking(int fd) {
	if (connect_state_ == CONNECT_STATE_CONNECT) {
		OnConnected();
	} 
}

}

