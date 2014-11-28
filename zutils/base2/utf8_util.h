// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef BASE2_UTF8_UTIL_H_
#define BASE2_UTF8_UTIL_H_

#include "base/basictypes.h"
#include "base/string_piece.h"
#include "base/sys_string_conversions.h"

namespace base {

inline std::string UTF8ToNativeMB(const StringPiece& utf8) {
	return SysWideToNativeMB(SysUTF8ToWide(utf8));	
}

inline std::string NativeMBToUTF8(const StringPiece& native_mb) {
	return SysWideToUTF8(SysNativeMBToWide(native_mb));	
}

}

#endif

