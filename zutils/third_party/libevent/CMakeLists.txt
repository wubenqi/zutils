ADD_DEFINITIONS(-DHAVE_CONFIG_H)

if (__linux__)             
	INCLUDE_DIRECTORIES(./linux)
endif()

SET(SRC_LIST buffer.c
            evbuffer.c
            evdns.c
            event.c
            event_tagging.c
            evrpc.c
            evutil.c
            http.c
            log.c
            poll.c
            select.c
            signal.c
            strlcpy.c)

if (__linux__)
	SET(PLATFORM_LIST epoll.c epoll_sub.c)
endif()
			
if (__linux__)             
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
endif()


SET(LIBRARY_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_LIB})

ADD_LIBRARY(libevent STATIC ${SRC_LIST} ${PLATFORM_LIST})

IF(CMAKE_SYSTEM MATCHES Windows)
  
ENDIF(CMAKE_SYSTEM MATCHES Windows)


