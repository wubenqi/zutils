SET(SRC_LIST adler32.c
			 compress.c
			 crc32.c
			 deflate.c
			 infback.c
			 inffast.c
			 inflate.c
			 inftrees.c
			 trees.c
			 uncompr.c
			 zutil.c
             )

if (WIN32)             
else()
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=x86-64 -mtune=i686")
endif()


SET(LIBRARY_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_LIB})

ADD_LIBRARY(zlib STATIC ${SRC_LIST})

IF(CMAKE_SYSTEM MATCHES Windows)
  
ENDIF(CMAKE_SYSTEM MATCHES Windows)


