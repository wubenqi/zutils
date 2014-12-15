INCLUDE_DIRECTORIES(/usr/include/mysql/)

SET(SRC_LIST conn_pool_manager.cc
			database_util.cc
			mysql/mysql_dll.cc
			mysql/mysql_database.cc)

SET(LIBRARY_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_LIB})

ADD_LIBRARY(db3 STATIC ${SRC_LIST})
	  