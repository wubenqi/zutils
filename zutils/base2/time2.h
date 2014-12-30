// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef BASE2_TIME2_H_
#define BASE2_TIME2_H_
#pragma once

#include "base/time/time.h"

namespace base {

#ifdef OS_WIN

inline uint32 NowMSTime() { return GetTickCount(); }

#else

inline uint32 NowMSTime() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

#endif

}

#endif
