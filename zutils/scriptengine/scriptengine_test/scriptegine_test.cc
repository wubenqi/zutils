// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//
// Lua½Å±¾ÒýÇæ
//

#include "scriptengine/script_engine.h"

#include <iostream>

#include "base/command_line.h"
#include "base/at_exit.h"
#include "base/logging.h"
#include "base/md5.h"
#include "base2/shared_ptr.h"

#include "url/gurl.h"


std::string MD5String2(const std::string& str) {
  base::MD5Digest digest;
  base::MD5Sum(str.c_str(), str.length(), &digest);
  return base::MD5DigestToBase16(digest);
}

lua_tinker::table TableTest(lua_tinker::table t) {
  //base::MD5Digest digest;
  //base::MD5Sum(str.c_str(), str.length(), &digest);
  //return base::MD5DigestToBase16(digest);
  return t;
}

// void Luabind_Base_Register(lua_State* L) {
//   lua_tinker::def(L, "MD5String", &MD5String2);
//   lua_tinker::def(L, "UTF8ToNativeMB", &UTF8ToNativeMB);
// }

class T {
public:
  T() {
    std::cout << "T::T()" << std::endl;
  }
  ~T() {
    std::cout << "T::~T()" << std::endl;
  }
  void Print() {
    std::cout << "T::Print()" << std::endl;
  }
};

shared_ptr<T> CreateTSharedPtrTest() {
  return shared_ptr<T>(new T());
}

T CreateTTest() {
  return T();
}

void CppTestingRegister(ScriptEngine* script_engine) {
#undef RegisterClass
  script_engine->RegisterClass<T>("T");
  script_engine->RegisterClassFuncnction<T>("Print", &T::Print);

  script_engine->RegisterClass<shared_ptr<T> >("SharedPtr_T");
  script_engine->RegisterClassFuncnction<shared_ptr<T> >("Get", &shared_ptr<T>::get);

  script_engine->RegisterFunction("MD5String", &MD5String2);
  script_engine->RegisterFunction("TableTest", &TableTest);

  script_engine->RegisterFunction("CreateTSharedPtrTest", &CreateTSharedPtrTest);
  script_engine->RegisterFunction("CreateTTest", &CreateTTest);

}



#include "http_client/base_http_fetch.h"
#include "http_client/http_fetch_factory.h"

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

/*
  ScriptEngine script_engine;
  script_engine.Initialize();
  CppTestingRegister(&script_engine);
  script_engine.DoFile(argv[1]);
  script_engine.CallFunction<int>("DoMain");
  script_engine.Destroy();
 */


  ScriptEngine script_engine;
  script_engine.Initialize();
  CppTestingRegister(&script_engine);


  script_engine.DoFile(argv[1]);


  // lua_tinker::table haha(script_engine.GetLuaState(),"haha");
  script_engine.CallFunction<int>("DoMain");

  {
  //  lua_tinker::table ret = lua_tinker::call<lua_tinker::table>(script_engine.GetLuaState(), "return_table", "give me a table !!");
  //  script_engine.CallFunction<void>("print_table", ret); 
  //  script_engine.CallFunction<void>("print_table2", ret); 
  }

  // script_engine.CallFunction<int>("DoMain");
  // lua_tinker::table haha(L,"haha");

  script_engine.Destroy();

  // lua_tinker::table haha(L,"haha");


//   GURL url("http://localhost/");
// 
//   HttpRequestInfo req;
//   HttpResponseInfo rsp;
//   req.method = "GET";
//   req.url = url;
// 
//   BaseHttpFetch*  http_client = HttpFetchFactory::Create(); 
//   http_client->Fetch(req, rsp);

  return 0;
}