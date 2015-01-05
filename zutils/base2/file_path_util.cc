// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "base2/file_path_util.h"

#include "base/strings/sys_string_conversions.h"

namespace file_path_util {

#if defined(OS_POSIX)
// See file_path.h for a discussion of the encoding of paths on POSIX
// platforms.  These *Hack() functions are not quite correct, but they're
// only temporary while we fix the remainder of the code.
// Remember to remove the #includes at the top when you remove these.

base::FilePath FromStringHack(const std::string& string) {
  return base::FilePath(string);
}
std::string ToStringHack(const base::FilePath& file_path) {
	return file_path.value();
}

#elif defined(OS_WIN)

base::FilePath FromStringHack(const std::string& string) {
  return base::FilePath(base::SysNativeMBToWide(string));
}
std::string ToStringHack(const base::FilePath& file_path) {
	return base::SysWideToNativeMB(file_path.value());
}

#endif

}

