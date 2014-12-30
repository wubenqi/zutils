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

std::string lua_buffer = 
"function Test()\n"
"  print('Hello scriptengine')\n"
"  t1 = T1()\n"
"  t1:Debug()\n"
"  t2 = T1(1)\n"
"  t2:Debug()\n"
"  t3 = T1(2,3)\n"
"  t3:Debug()\n"
"end";


struct T1 {
public:
  T1() {
    t1_ = 0;
    t2_ = 0;
  }

  T1(int t1) {
    t1_ = t1;
    t2_ = 0;
  }

  T1(int t1, int t2) {
    t1_ = t1;
    t2_ = t2;
  }

  void Debug() {
    std::cout << "t1 = " << t1_ << std::endl;
    std::cout << "t2 = " << t2_ << std::endl;
  }

  int t1_;
  int t2_;
};

int main(int argc, char* argv[]) {
  ScriptEngine script_engine;
  script_engine.Initialize();

  script_engine.RegisterClass<T1>("T1");
  script_engine.RegisterClassConstructor<T1>();
  script_engine.RegisterClassConstructor<T1, int>();
  script_engine.RegisterClassConstructor<T1, int, int>();
  script_engine.RegisterClassFuncnction<T1>("Debug", &T1::Debug);

  script_engine.DoBuffer(lua_buffer);
  script_engine.CallFunction<int>("Test");
  script_engine.Destroy();
  return 0;
}