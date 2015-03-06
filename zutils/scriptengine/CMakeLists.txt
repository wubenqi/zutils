INCLUDE_DIRECTORIES(./lua-5.2.3/src)

SET(SRC_LIST 
		./lua_util.cc
		./script_engine.cc
		./luatinker/lua_tinker.cpp
	)

SET(LIBRARY_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_LIB})

ADD_LIBRARY(script_engine STATIC ${SRC_LIST})
	  