SET(SRC_LIST dynamic_annotations.c dynamic_annotations.h)

SET(LIBRARY_OUTPUT_PATH ${TF_CURRENT_BINARY_PATH_LIB})

ADD_LIBRARY(dynamic_annotations STATIC ${SRC_LIST})
