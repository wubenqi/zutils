#include "base/basictypes.h"

#include "base/at_exit.h"
#include "base/logging.h"
#include "base/bind.h"
#include "base/files/file_path.h"
#include "base/base_paths.h"
#include "base/command_line.h"
#include "base/path_service.h"

#include "base/message_loop/message_loop.h"


using namespace base;
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
    message_loop->PostTask(FROM_HERE, base::Bind(&OnShutdownDaemon, message_loop));
  }
  
}


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

int main(int argc, char* argv[]) {
  base::AtExitManager at_exit;

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
  SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
  MessageLoop   message_loop;
  g_message_loop = &message_loop;
  //MessageLoop::current()->Run();
  message_loop.Run();

  g_message_loop = NULL;

  LOG(INFO) << "quit!!!";

  return 0;
}