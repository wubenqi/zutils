// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef BASE2_HASH_TABLES2_H_
#define BASE2_HASH_TABLES2_H_
#pragma once

#include "base/hash_tables.h"

#if defined(COMPILER_MSVC)
namespace base {
using stdext::hash_multimap;
using stdext::hash_multiset;
}
#elif defined(COMPILER_GCC)
namespace base {
using __gnu_cxx::hash_multimap;
using __gnu_cxx::hash_multiset;
}  // namespace base
#endif  // COMPILER

#endif  // BASE_HASH_TABLES2_H_
