// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "base/at_exit.h"
#include "base/threading/platform_thread.h"
#include "base/logging.h"
#include "base/bind.h"
#include "base/files/file_path.h"
#include "base/base_paths.h"
#include "base/command_line.h"
#include "base/path_service.h"

#include "net/base/winsock_init.h"
#include "net/engine/tcp_client.h"
#include "net/engine/tcp_server.h"

#include "net/message_loop/message_pump_libevent2.h"

#include "net/codec/znet/znet_codec.h"
#include "net/http/http_request_codec.h"

#include <iostream>

using namespace net;
using namespace base;

class TestServer :
  public net::IOHandler::Delegate {
public:
  TestServer(base::MessageLoop* message_loop)
		: acc_(message_loop, this) {
		acc_.Create("127.0.0.1", "12345", false);
	}

	virtual int OnNewConnection(const IOHandlerPtr& ih) {
    std::cout << "TestServer::OnNewConnection" << std::endl;
		return 0;
	}

  virtual int  OnDataReceived(const IOHandlerPtr& ih, IOBuffer* data, const base::Time& read_time) {
    std::cout << base::PlatformThread::CurrentId() << "@TestServer::OnDataReceived ==> " << data->Peek();// << std::endl;
    ih->SendInternal(data->Peek(), data->ReadableBytes());
    data->RetrieveAll();
    return 0;
  }

	virtual int OnConnectionClosed(const IOHandlerPtr& ih) { 
    std::cout << "TestServer::OnConnectionClosed" << std::endl;
		return 0;
	}


private:
	TCPServer acc_;
};

class TestConn :
  public net::IOHandler::Delegate {
public:
  TestConn(base::MessageLoop* message_loop) :
    conn_(message_loop, this) {
		conn_.Connect("127.0.0.1", "12345", true);
	}

  virtual int  OnNewConnection(const IOHandlerPtr& ih) {
    std::cout << "TestConn::OnNewConnection" << std::endl;
    ih->SendInternal("111\r\n", 6);
    return 0;
  }

  virtual int  OnDataReceived(const IOHandlerPtr& ih, IOBuffer* data, const base::Time& read_time) {
    // std::cout << "TestConn::OnDataReceived: data_len = " << len << std::endl;
    ih->SendInternal(data->Peek(), data->ReadableBytes());
    data->RetrieveAll();
    // ih->SendInternal(data, len);
    return 0;
  }

  virtual int  OnConnectionClosed(const IOHandlerPtr& ih) {
    std::cout << "OnDataReceived::OnConnectionClosed" << std::endl;
    return 0;
  }

private:
	TCPClient conn_; //(new net::TCPAcceptor(&net_engine_manager, &ih)); 
};

class ZNetServerTest : public net::ZNetCodec::Delegate {
public:
  ZNetServerTest(base::MessageLoop* message_loop)
    : codec_(this),
      acc_(message_loop, &codec_) {

    acc_.Create("127.0.0.1", "12345", false);
  }

  virtual int  OnZNetNewConnection(const IOHandlerPtr& ih) {
    std::cout << "ZNetServerTest::OnNewConnection" << std::endl;
    return 0;
  }

  virtual int  OnZNetDataReceived(const IOHandlerPtr& ih, const PacketPtr& packet, base::Time receive_time) {
    std::cout << "ZNetServerTest::OnZNetDataReceived: " << packet->GetBodyData() << std::endl;
    codec_.SendPacket(ih, packet);
    return 0;
  }

  virtual int  OnZNetConnectionClosed(const IOHandlerPtr& ih) {
    std::cout << "ZNetServerTest::OnZNetConnectionClosed" << std::endl;
    return 0;
  }

private:
  ZNetCodec codec_;
  TCPServer acc_; //(new net::TCPAcceptor(&net_engine_manager, &ih)); 
};

class ZNetConTest : public net::ZNetCodec::Delegate {
public:
  ZNetConTest(base::MessageLoop* message_loop)
    : codec_(this),
      conn_(message_loop, &codec_) {

      conn_.Connect("127.0.0.1", "12345", false);
  }

  virtual int  OnZNetNewConnection(const IOHandlerPtr& ih) {
    std::cout << "ZNetServerTest::OnNewConnection" << std::endl;

    PacketPtr packet(new Packet(1, 4));
    // uint16 cmd_type, uint32 data_len);
    memcpy(packet->GetBodyMutableData(), "111", 4);
    codec_.SendPacket(ih, packet);

    return 0;
  }

  virtual int  OnZNetDataReceived(const IOHandlerPtr& ih, const PacketPtr& packet, base::Time receive_time) {
    // std::cout << "ZNetServerTest::OnZNetDataReceived: " << packet->GetBodyData() << std::endl;

    // std::cout << "TestConn::OnZNetDataReceived" << std::endl;
    codec_.SendPacket(ih, packet);

    return 0;
  }

  virtual int  OnZNetConnectionClosed(const IOHandlerPtr& ih) {
    std::cout << "ZNetServerTest::OnZNetConnectionClosed" << std::endl;
    return 0;
  }

private:
  ZNetCodec codec_;
  TCPClient conn_; //(new net::TCPAcceptor(&net_engine_manager, &ih)); 
};

class HttpServerTest : public HttpRequestCodec::Delegate {
public:
  HttpServerTest(base::MessageLoop* message_loop)
    : codec_(this),
      acc_(message_loop, &codec_) {

      acc_.Create("0.0.0.0", "8000", false);
  }

  virtual int OnHttpConnection(const IOHandlerPtr& ih) {
    LOG(INFO) << "OnHttpConnection(): id = " << ih->io_handler_id();
    return 0;
  }

  virtual int OnHttpRequest(const IOHandlerPtr& ih, const HttpServerRequestInfo& request, base::Time receive_time) {
    LOG(INFO) << "OnHttpRequest(): id = " << ih->io_handler_id();
    codec_.Send500(ih, "tttttttttttttttttt");
    return 0;
  }

  virtual int OnHttpConnectionClosed(const IOHandlerPtr& ih) {
    LOG(INFO) << "OnHttpConnectionClosed(): id = " << ih->io_handler_id();
    return 0;
  }

private:
  HttpRequestCodec codec_;
  TCPServer acc_; //(new net::TCPAcceptor(&net_engine_manager, &ih)); 
};


int main(int argc, char* argv[]) {
	base::AtExitManager exit;

  CommandLine::Init(argc, argv);
  FilePath exe;
  PathService::Get(base::FILE_EXE, &exe);
  FilePath log_filename = exe.ReplaceExtension(FILE_PATH_LITERAL("log"));
  logging::LoggingSettings logging_settings;
  logging_settings.log_file = log_filename.value().c_str();
  logging_settings.lock_log = logging::LOCK_LOG_FILE;
  logging_settings.delete_old = logging::APPEND_TO_OLD_LOG_FILE;
  logging_settings.logging_dest = logging::LOG_TO_ALL;
  logging::InitLogging(logging_settings);
  // We want process and thread IDs because we may have multiple processes.
  // Note: temporarily enabled timestamps in an effort to catch bug 6361.
  logging::SetLogItems(false, true, true, true);

  LOG(INFO) << "run...";

	#if defined(OS_WIN)
	net::EnsureWinsockInit();
	#endif  // defined(OS_WIN)

	// Do work here.
  base::MessageLoop loop(scoped_ptr<base::MessagePump>(new base::MessagePumpLibevent2()));

  HttpServerTest server(&loop);
	//ZNetServerTest server(&loop);

 // std::vector<ZNetConTest*> conns;
 // for (size_t i=0; i<10; ++i) {
 //   conns.push_back(new ZNetConTest(&loop));
 //   // TestConn test_conn(&engine_manager);
 // }

  base::MessageLoop::current()->Run();

  //for (size_t i=0; i<conns.size(); ++i) {
  //  delete conns[i];
  //}

  return 0;
}
