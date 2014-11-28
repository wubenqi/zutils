// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef BASE2_FILE_PATH_UTIL_H_
#define BASE2_FILE_PATH_UTIL_H_
#pragma once

#include "base/file_path.h"

namespace file_path_util {

std::string ToStringHack(const FilePath& file_path);
FilePath FromStringHack(const std::string& str);

}

#endif
