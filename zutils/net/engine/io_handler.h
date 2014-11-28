// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_IO_HANDLER_H_
#define NET_ENGINE_IO_HANDLER_H_
#pragma once

#include "net/base/net_types.h"
#include "net/base/io_buffer.h"
#include "net/engine/socket_ops.h"

#include "base/memory/ref_counted.h"
#include "base/message_loop.h"

namespace net {

class Reactor;
class IOHandlerDelegate {
public:
  virtual ~IOHandlerDelegate() {}
  virtual int OnNewConnection() { return 0; }
  virtual int OnDataReceived(const char* data, int len) { return 0; }
  virtual int OnConnectionClosed() { return 0; }
};

class IOHandler : 
  public base::RefCountedThreadSafe<IOHandler>,
  public MessageLoopForIO::Watcher,
  public IOHandlerDelegate {
public:
  class IOHandlerCallBack {
  public:
    virtual int OnNewConnection(IOHandler* ih) {return 0;}
    virtual int OnConnectionClosed(IOHandler* ih) { return 0;}
  };

  IOHandler(SOCKET socket, Reactor* reactor);
  virtual ~IOHandler();

  inline SOCKET socket() { return socket_; }
  inline void SetIOHandlerCallBack(IOHandlerCallBack* call_back) {
    ih_call_back_ = call_back;
  }

  void Create();

  virtual void OnCreated();
  virtual void Close( );
  virtual void Read();
  virtual void SendInternal(const char* bytes, int len);
  virtual void CloseSocket(SOCKET s);

  virtual void SetUserData(void* user_data) {}

  virtual void OnTaskDataReceived(const char* data, uint32 data_len) {}

  void AsyncSendIOBuffer(IOBufferPtr io_buffer);

  void AsyncSend(const char* io_buffer, uint32 io_buffer_len);

  // void AddIOHandlerDelegate(IOHandlerDelegate* ih_delegate) { ih_delegates_.push_front(ih_delegate); }

  const std::string& GetAddressString() {
    return remote_string_;
  }

  Reactor* GetReactor() { return reactor_; }
  int GetReactorID();
  inline SessionID GetSessionID() {
    return session_id_;
  }

protected:
  friend class Reactor;
  void SendInternal2(scoped_refptr<IOBuffer> io_buffer, uint32 io_buffer_len);

  // Send data to the socket.
  void Send(const char* bytes, int len);
  void Send(const std::string& str);

  // Pass any value in case of Windows, because in Windows
  // we are not using state.
  void WatchSocket(WaitState state);
  void UnwatchSocket();


  //// ObjectWatcher delegate
  //virtual void OnObjectSignaled(HANDLE object);
  //base::win::ObjectWatcher watcher_;
  //HANDLE socket_event_;

  // Called by MessagePumpLibevent when the socket is ready to do I/O
  virtual void OnFileCanReadWithoutBlocking(int fd);
  virtual void OnFileCanWriteWithoutBlocking(int fd);
  WaitState wait_state_;
  // The socket's libevent wrapper
  MessageLoopForIO::FileDescriptorWatcher watcher_;

protected:
  SOCKET socket_;
  std::string remote_string_;

private:
  Reactor *reactor_;
  IOHandlerCallBack* ih_call_back_;
  SessionID session_id_;
  //std::list<IOHandlerDelegate *> ih_delegates_;
};

}

#endif
