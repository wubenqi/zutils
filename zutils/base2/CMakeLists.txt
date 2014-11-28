ADD_DEFINITIONS(-DTIXML_USE_STL)

SET(SRC_LIST xml/xml_attributes.cc
			xml/xml_parser.cc
			xml/xml_serializer.cc
			xml/tinyxml_parser.cc
			xml/xml_lite.cc
			xml/xml_parser_module.cc
			xml/third_party/tinyxml/tinystr.cpp
			xml/third_party/tinyxml/tinyxmlerror.cpp
			xml/third_party/tinyxml/tinyxml.cpp
			xml/third_party/tinyxml/tinyxmlparser.cpp
			daemon/base_daemon.cc
			io_buffer2.cc
			arg.cc
			well512_random.cc
			well512_random.h
			eventable_object.cc
			line_pool.cc
			rand_util2.cc
			uuid.cc
			callback.cc
			event_manager.cc
			markup_stl.cc
			my_ini.cc
			recursive_mutex.cc
			xml_ini_file.cc
			file_path_util.cc
			object_core.cc
			zlib_compress.cc)

SET(LIBRARY_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_LIB})

ADD_LIBRARY(base2 STATIC ${SRC_LIST})
	  