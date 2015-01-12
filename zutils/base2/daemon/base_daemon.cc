// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "base2/daemon/base_daemon.h"

//#include <ostream>

#include "base/logging.h"
#include "base/memory/singleton.h"
#include "base/command_line.h"
#include "base/path_service.h"
#include "base/threading/platform_thread.h"

#include "base/bind.h"
//#include "base/sys_info.h"
//
//#include "net/base/winsock_init.h"
//#include "net/engine/net_engine_manager.h"


#if !defined(OS_WIN)
#include <signal.h>
#if 0
#include "debugger/DbgLib.h"
#endif
#endif

//#define UNIT_TEST
//#include "base/process_util.h"
//
//#define DEFAULT_LOOP_TIME	10

namespace base {

//BaseDaemon* g_base_daemon = NULL;
//static bool g_want_exit = false;

MessageLoop* g_message_loop = NULL;

void OnShutdownDaemon(MessageLoop* message_loop) {
  if (message_loop) {
    message_loop->Quit();
  }
}

void DoShutdownDaemon(MessageLoop* message_loop) {
  if (message_loop) {
    message_loop->PostNonNestableTask(FROM_HERE, base::Bind(&OnShutdownDaemon, message_loop));
  }
  
}


#if defined OS_WIN
BOOL WINAPI ConsoleCtrlHandler(DWORD ctrl_type) {
  switch (ctrl_type) {
  case CTRL_C_EVENT:
  case CTRL_BREAK_EVENT:
  case CTRL_CLOSE_EVENT:
  case CTRL_SHUTDOWN_EVENT:
    DoShutdownDaemon(g_message_loop);
    //if (g_base_daemon) {
    //  g_want_exit = true;
    //  // g_base_daemon->DoShutDown();
    //}
    //if (g_spider_engine_manager) {
    //  // g_message_loop->PostTask();
    //  //g_message_loop->PostTask(FROM_HERE, new MessageLoop::QuitTask);
    //  g_spider_engine_manager->Shutdown2();
    //}
    // console_ctrl_function();
    return TRUE;
  default:
    return FALSE;
  }
}
#else
void SignalHandler(int signum) {
  switch(signum) {
    case SIGTERM:
    case SIGINT:
    case SIGHUP:
      DoShutdownDaemon(g_message_loop);
      break;
  }
}
#endif

// exception callback function
void ExceptionCallback(void*) {
}

BaseDaemon::BaseDaemon() 
	: running_(false),
    message_loop_(NULL) /*
  , ALLOW_THIS_IN_INITIALIZER_LIST(timer_manager_(this, &message_loop_)) */{

//#if defined(OS_WIN)
//	net::EnsureWinsockInit();
//#endif  // defined(OS_WIN)

  //if (reactor_pool_size <=0 ) {
  //  eng_mgr_.reset(new NetEngineManager(base::SysInfo::NumberOfProcessors()));
  //} else {
  //  eng_mgr_.reset(new NetEngineManager(reactor_pool_size));
  //}
  // g_base_daemon = this;
  //message_loop_ = MessageLoop::current();
}

BaseDaemon::~BaseDaemon( ) {
  if (message_loop_) {
    delete message_loop_;
    message_loop_ = NULL;
  }
}

int BaseDaemon::LoadConfig( const FilePath& xml_ini_file ) {
	return 0;
}

int	BaseDaemon::Initialize( int argc, char** argv ) {
	return 0;
}

//void BaseDaemon::DoShutDown() {
//  message_loop_.PostTask(FROM_HERE, base::Bind(&BaseDaemon::OnShutDown, base::Unretained(this)));
//}
//
//void BaseDaemon::OnShutDown() {
//  //message_loop_.DeleteSoon(;)
//  message_loop_.Quit();
//}

int BaseDaemon::Run( ) {
  message_loop_->Run();
  return 0;
}

int BaseDaemon::Run2( ) {
	// Set console control handler to allow server to be stopped.
	//console_ctrl_function = boost::bind(&BaseDaemon::Destroy2, this);

#if defined(OS_WIN)
	SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
  running_ = true;
#else
  signal(SIGPIPE, SIG_IGN);
  signal(SIGTERM, SignalHandler);
  signal(SIGINT, SignalHandler);
  signal(SIGHUP, SignalHandler);
  running_ = true;
#endif

  g_message_loop = message_loop();

  Run();
  // message_loop_.Run();

  //while (!g_want_exit) {
  //  base::PlatformThread::Sleep(base::TimeDelta::FromMilliseconds(10));
  //  // usleep(1000*10);  // 10 ms
  //}

	return true;
}

int BaseDaemon::Destroy( ) {
  //timer_manager_.Destroy();
	return 0;
}

int BaseDaemon::DoMain( int argc, char** argv ) {
	CommandLine::Init(argc, argv);
	FilePath exe;
	PathService::Get(base::FILE_EXE, &exe);
  exe_path_ = exe.DirName();
	FilePath log_filename = exe.ReplaceExtension(FILE_PATH_LITERAL("log"));

  logging::LoggingSettings logging_settings;
  logging_settings.log_file = log_filename.value().c_str();
  logging_settings.lock_log = logging::LOCK_LOG_FILE;
  logging_settings.delete_old = logging::APPEND_TO_OLD_LOG_FILE;
  logging_settings.logging_dest = logging::LOG_TO_ALL;
  logging::InitLogging(logging_settings);

// 	logging::InitLogging(log_filename.value().c_str(),
// 		logging::LOG_TO_BOTH_FILE_AND_SYSTEM_DEBUG_LOG,
// 		logging::LOCK_LOG_FILE,
// 		logging::DELETE_OLD_LOG_FILE,
// 		logging::DISABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS);

	// We want process and thread IDs because we may have multiple processes.
	// Note: temporarily enabled timestamps in an effort to catch bug 6361.
	logging::SetLogItems(false, true, true, true);

#if 0
//#if !defined(OS_WIN)
  // enable exception handlers
  DbgLib::CDebugFx::SetExceptionHandler(true);
  // install an exception callback
  DbgLib::CDebugFx::SetExceptionCallback(ExceptionCallback, NULL);
#endif

	//CHECK(base::EnableInProcessStackDumping());

  base::FilePath ini_filename = exe.ReplaceExtension(FILE_PATH_LITERAL("xml"));
  CommandLine* command_line = CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch("xml")) {
    ini_filename = exe.DirName().AppendASCII(command_line->GetSwitchValueASCII("xml"));
  }

	LOG(INFO) << "Load config " << file_path_util::ToStringHack(ini_filename) << " ...";

	if( 0 != LoadConfig( ini_filename ) ) {
		LOG(ERROR) << "Load config: " << file_path_util::ToStringHack(ini_filename) << " error";
		return 2;
	}

	// eng_mgr_->Run();
	
	//if (0!=New()) {
	//	LOG(ERROR) << "error New: " << ini_file_;
	//	return 3;
	//}

	LOG(INFO) << "Initialize...";
	if( 0 != Initialize( argc, argv ) ) {
		LOG(ERROR) << "error initialize";
		return 4;
	}

	LOG(INFO) << "Running...";
	Run2( );
	//eng_mgr_.run();

	LOG(INFO) << "Destroy...";
	Destroy( );

	LOG(INFO) << "Shutdown...";
	return 0;
}

}

