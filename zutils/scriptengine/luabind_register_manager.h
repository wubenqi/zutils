// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef SCRIPTENGINE_LUABIND_REGISTER_MANAGER_H_
#define SCRIPTENGINE_LUABIND_REGISTER_MANAGER_H_

#include "base/memory/singleton.h"
#include "scriptengine/luabind_register_func.h"

class LuabindRegisterManager {
public:
  static LuabindRegisterManager* GetInstance() {
    return Singleton<LuabindRegisterManager>::get();
  }

  void RegisterLuabindRegisterFunc(const std::string& module_name, LuabindRegister_Func func) {
    LuabindRegisterFuncListMap::iterator it = registers_.find(module_name);
    if (it == registers_.end()) {
      registers_[module_name].push_back(func);
    } else {
      it->second.push_back(func);
    }
	}

	LuabindRegisterFuncListMap& MutableRegisters() {
		return registers_;
	}

  void RestiterAll(lua_State* L) {
    for (LuabindRegisterFuncListMap::iterator it=registers_.begin(); it!=registers_.end(); ++it) {
      LuabindRegisterFuncList& funcs = it->second;
      for (size_t i=0; i<funcs.size(); ++i) {
        funcs[i](L);
      }
    }
  }

private:
	friend struct DefaultSingletonTraits<LuabindRegisterManager>;

  LuabindRegisterManager() {}
	~LuabindRegisterManager() {}

  LuabindRegisterFuncListMap registers_;
};


#endif // SCRIPTENGINE_LUABIND_PB_REGISTER_MANAGER_H_
