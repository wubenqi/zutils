// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef BASE_ZLIBCOMPRESS_H_
#define BASE_ZLIBCOMPRESS_H_

#include "base/basictypes.h"

namespace base {

class ZlibCompress {
public:
	static void Compress(const char* in_data, uint32 in_len, char** out_data, uint32* out_len);
	static bool UnCompress(const char* in_data, uint32 in_len, char** out_data, uint32* out_len);

private:
	static int Compress1(const char* in_data, uint32 in_len, char** out_data, uint32* out_len);
	static char* UnCompress1(const char* in_data, uint32 in_len, uint32* out_len);

	DISALLOW_IMPLICIT_CONSTRUCTORS(ZlibCompress);
};

}

#endif
