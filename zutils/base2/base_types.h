// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef BASE_BASE_TYPES_H_
#define BASE_BASE_TYPES_H_
#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

#include "base/basictypes.h"

#undef ATTRIBUTE_ALWAYS_INLINE
#if defined(__GNUC__) && (__GNUC__ > 3 ||(__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
// For functions we want to force inline.
// Introduced in gcc 3.1.
#define ATTRIBUTE_ALWAYS_INLINE __attribute__ ((always_inline))
#else
// Other compilers will have to figure it out for themselves.
#define ATTRIBUTE_ALWAYS_INLINE
#endif

#undef ATTRIBUTE_DEPRECATED
#ifdef __GNUC__
// If the method/variable/type is used anywhere, produce a warning.
#define ATTRIBUTE_DEPRECATED __attribute__((deprecated))
#else
#define ATTRIBUTE_DEPRECATED
#endif

#ifndef PREDICT_TRUE
#ifdef __GNUC__
// Provided at least since GCC 3.0.
#define PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#else
#define PREDICT_TRUE
#endif
#endif


//////////////////////////////////////////////////////////////////////////

//根据指针值删除内存
#ifndef SAFE_DELETE
#define SAFE_DELETE(x)	if( (x)!=NULL ) { delete (x); (x)=NULL; }
#endif
//根据指针值删除数组类型内存
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x)	if( (x)!=NULL ) { delete[] (x); (x)=NULL; }
#endif
//根据指针调用free接口
#ifndef SAFE_FREE
#define SAFE_FREE(x)	if( (x)!=NULL ) { free(x); (x)=NULL; }
#endif
//根据指针调用Release接口
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)	if( (x)!=NULL ) { (x)->Release(); (x)=NULL; }
#endif


typedef std::map<std::string, std::string> NameValuePairList;

inline uint64 MakeInt64(uint32 low, uint32 high) {
  return ((uint64)(((uint32)(low)) | ((uint64)((uint32)(high))) << 32));
}

inline uint32 GetLowInt32ByInt64(uint64 v) {
  return v & 0xFFFFFFFF;
}

inline uint32 GetHighInt32ByInt64(uint64 v) {
  return (v >> 32) & 0xFFFFFFFF;
}

inline uint32 MakeInt32(uint16 low, uint16 high) {
  return ((uint32)(((uint16)(low) << 16) | ((uint32)((uint16)(high)))));
}

inline uint16 GetLowInt16ByInt32(uint32 v) {
  return (v >> 16) & 0xFFFF;
}

inline uint16 GetHighInt16ByInt32(uint32 v) {
  return v & 0xFFFF;
}

#endif  // BASE_BASE_TYPES_H_
