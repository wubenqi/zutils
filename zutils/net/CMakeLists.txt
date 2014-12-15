SET(SRC_LIST base/byte_stream.cc
			base/io_buffer.cc
			base/net_errors_posix.cc
			base/data_io_buffer.cc
			base/net_errors.cc
			engine/io_handler.cc
			engine/reactor.cc
			engine/tcp_acceptor.cc
			engine/net_engine_manager.cc
			engine/socket_ops.cc
			engine/tcp_connector.cc
			znet/packet.cc
			znet/znet_io_handler.cc
			teamtalk/teamtalk_packet.cc
			teamtalk/teamtalk_io_handler.cc)

SET(LIBRARY_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_LIB})

ADD_LIBRARY(net STATIC ${SRC_LIST})
	  