// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "c1000k/c1000k_client.h"

#include "base/at_exit.h"
#include "base/sys_info.h"
#include "base/strings/string_number_conversions.h"
#include "base/command_line.h"

#if defined(OS_WIN)
#include "net/base/winsock_init.h"
#endif

#include "net/message_loop/message_pump_libevent2.h"

C1000kClient::C1000kClient() 
: BaseDaemon() {
  message_loop_ = new base::MessageLoop(scoped_ptr<base::MessagePump>(new base::MessagePumpLibevent2()));
}

C1000kClient::~C1000kClient() {
}

int C1000kClient::LoadConfig( const base::FilePath& xml_ini_file ) {
  return 0;
}

int	C1000kClient::Initialize( int argc, char** argv ) {
  // 客户端连接处理

  std::string server_addr("127.0.0.1");
  std::string server_port("9100");
  int conn_count = 10240;

  CommandLine* command_line = CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch("server_addr")) {
    server_addr = command_line->GetSwitchValueASCII("server_addr");
  }
  if (command_line->HasSwitch("server_port")) {
    server_port = command_line->GetSwitchValueASCII("server_port");
  }
  if (command_line->HasSwitch("conn_count")) {
    base::StringToInt(command_line->GetSwitchValueASCII("conn_count"), &conn_count);
  }

  codec_.reset(new net::LengthHeaderCodec(this));

  for (int i=0; i<conn_count; ++i) {
    net::TCPClient* conn = new net::TCPClient(message_loop(), codec_.get());
    conn->Connect(server_addr, server_port, false);
    // servers_.push_back(server);
  }

  return 0;
}

int C1000kClient::Destroy() {
  return 0;
}

int C1000kClient::OnLengthHeaderNewConnection(const net::IOHandlerPtr& ih) {
  LOG(INFO) << "OnLengthHeaderNewConnection()";
  return 0;
}

int C1000kClient::OnLengthHeaderDataReceived(const net::IOHandlerPtr& ih, const base::StringPiece& packet, base::Time receive_time) {
  return 0;
}

int C1000kClient::OnLengthHeaderConnectionClosed(const net::IOHandlerPtr& ih) {
  LOG(INFO) << "OnLengthHeaderConnectionClosed()";
  return 0;
}

//////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
  base::AtExitManager at_exit_manager;

#if defined(OS_WIN)
  net::EnsureWinsockInit();
#endif

  C1000kClient daemon;
  return daemon.DoMain(argc, argv);
}

