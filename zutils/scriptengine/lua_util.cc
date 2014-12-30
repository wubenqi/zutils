// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//
// Lua脚本引擎
//

#include "scriptengine/lua_util.h"

#include <iostream>

void LuaDumpStackHelper (lua_State* L) {
	int top = lua_gettop(L);
	std::cout << ">>>> [LuaDumpStackHelper]: Return " << top << " Value" << std::endl;
	for (int i = 1; i <= top; i++) {
		/* repeat for each level */
		int t = lua_type(L, i);
		// const char* type_name = lua_typename(L, t);
		switch (t) {
		case LUA_TSTRING:  /* strings */
			std::cout << "\t[" << lua_typename(L, t) << "], " << lua_tostring(L, i) << std::endl;
			break;
		case LUA_TBOOLEAN:  /* booleans */
			std::cout << "\t[" << lua_typename(L, t) << "], " << (lua_toboolean(L, i) ? "true" : "false") << std::endl;
			break;
		case LUA_TNUMBER:  /* numbers */
			std::cout << "\t[" << lua_typename(L, t) << "], " << lua_tonumber(L, i) << std::endl;
			break;
		case LUA_TTABLE:  /* table */
			{
				// 简单的遍历表的功能   
				// ***好像lua不保存表的元素的添加顺序***   
				std::cout << "\t["<< lua_typename(L, t) << "] "<< std::endl;

				// 压入第一个键   
				lua_pushnil(L);  /* 第一个 key */  
				int t = -2;   
				while (lua_next(L, t) != 0) {   
					/* 'key' (索引-2) 和 'value' (索引-1) */  
					const char* key = "unknown";   
					const char* value;   
					if(lua_type(L, -2) == LUA_TSTRING) {   
						key = lua_tostring(L, -2);   
						value = lua_tostring(L, -1);   
					} else if(lua_type(L, -2) == LUA_TNUMBER) {   
						// 因为lua_tostring会更改栈上的元素，   
						// 所以不能直接在key上进行lua_tostring   
						// 因此，复制一个key，压入栈顶，进行lua_tostring   
						lua_pushvalue(L, -2);   
						key = lua_tostring(L, -1);   
						lua_pop(L, 1);   
						value = lua_tostring(L, -1);   
					} else  { 
						value = lua_tostring(L, -1);   
					}   

					if (!value) {
						value = "nil";
					}
					std::cout <<"\t\tkey="<< key   
						<< ", value=" << value << std::endl;   
					/* 移除 'value' ；保留 'key' 做下一次迭代 */  
					lua_pop(L, 1);   
				}   

			}
			break;
		default:  /* other values */
			std::cout << "\t["<< lua_typename(L, t) << "], can't get val" << std::endl;
			break;

		}
	}
	std::cout << "[LuaDumpStackHelper] <<<<" << std::endl;

}