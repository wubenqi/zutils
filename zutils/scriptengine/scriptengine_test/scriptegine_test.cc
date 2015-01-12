// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//
// Lua½Å±¾ÒýÇæ
//

#include "scriptengine/script_engine.h"

// #include <iostream>
#include "base/command_line.h"
#include "base/at_exit.h"
#include "base/logging.h"
#include "base/md5.h"

std::string MD5String2(const std::string& str) {
  base::MD5Digest digest;
  base::MD5Sum(str.c_str(), str.length(), &digest);
  return base::MD5DigestToBase16(digest);
}

// void Luabind_Base_Register(lua_State* L) {
//   lua_tinker::def(L, "MD5String", &MD5String2);
//   lua_tinker::def(L, "UTF8ToNativeMB", &UTF8ToNativeMB);
// }

void CppTestingRegister(ScriptEngine* script_engine) {
  script_engine->RegisterFunction("MD5String", &MD5String2);
}

int main(int argc, char* argv[]) {
  base::AtExitManager at_exit;

//   CommandLine::Init(argc, argv);
// 
//   logging::LoggingSettings logging_settings;
//   logging_settings.logging_dest = logging::LOG_TO_SYSTEM_DEBUG_LOG;
//   logging::InitLogging(logging_settings);
//   logging::SetLogItems(false, true, true, true);

  if (argc != 2) {
    LOG(ERROR) << "Need lua file!!!";
    return -1;
  }

  ScriptEngine script_engine;
  script_engine.Initialize();
  CppTestingRegister(&script_engine);
  script_engine.DoFile(argv[1]);
  script_engine.CallFunction<int>("DoMain");
  script_engine.Destroy();

  return 0;
}