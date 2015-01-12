// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef SCRIPTENGINE_LUABIND_REGISTER_PB_H_
#define SCRIPTENGINE_LUABIND_REGISTER_PB_H_

#include <vector>
#include "base2/linked_map.h"

struct lua_State;
typedef void (*LuabindRegister_Func)(lua_State* L);

typedef std::vector<LuabindRegister_Func> LuabindRegisterFuncList;
typedef base::linked_map<std::string, LuabindRegisterFuncList> LuabindRegisterFuncListMap;


#endif
