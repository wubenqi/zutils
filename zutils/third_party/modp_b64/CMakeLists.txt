#INCLUDE_DIRECTORIES(../..)

SET(SRC_LIST modp_b64.cc)

if (WIN32)             
else()
  SET(CMAKE_C_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
  SET(CMAKE_CXX_FLAGS "-Wall -ggdb -pipe -march=i386 -mtune=i686")
endif()


SET(LIBRARY_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_LIB})

ADD_LIBRARY(modp_b64 STATIC ${SRC_LIST})

IF(CMAKE_SYSTEM MATCHES Windows)
  
ENDIF(CMAKE_SYSTEM MATCHES Windows)


