// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef BASE2_STRING_VECTOR_H_
#define BASE2_STRING_VECTOR_H_

//#include "base/basictypes.h"

#include <vector>
#include <string>

namespace base {

struct CStringVector {
  size_t size() const {
    return cstring_vector.size();
  }
  std::vector<const char*> cstring_vector;
};

struct StringVector {
  size_t size() const {
    return string_vector.size();
  }
  std::vector<std::string> string_vector;
};

}

#endif
