SET(SRC_LIST message_pdu_factory.cc
			message_pdu_factory_manager.cc
			message_pdu_util.cc)

SET(LIBRARY_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_LIB})

ADD_LIBRARY(message_pdu STATIC ${SRC_LIST})
	  