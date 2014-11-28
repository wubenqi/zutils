// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "base2/string_util2.h"

#include "base/stl_util.h"
#include "base/string_number_conversions.h"

const std::string* JoinString(const std::vector<uint32>& parts, char sep, std::string* s) {
  if (parts.empty())
    return NULL;

  STLClearObject(s);
  *s = base::UintToString(parts[0]);  
  std::vector<uint32>::const_iterator iter = parts.begin();
  ++iter;

  for (; iter != parts.end(); ++iter) {
    s->push_back(sep);
    s->append(base::UintToString(*iter));
  }

  return s;
}
