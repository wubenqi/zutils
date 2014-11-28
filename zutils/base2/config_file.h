// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef BASE2_CONFIG_FILE_H_
#define BASE2_CONFIG_FILE_H_

#include "base/basictypes.h"

#include <map>
#include "base2/string_vector.h"

namespace base {

class ConfigFile {
public:
  ConfigFile() {}
  ~ConfigFile() {}

  typedef std::pair<std::string, std::string> KeyValuePair;
  typedef std::vector<KeyValuePair> KeyValuePairList;
  typedef std::map<std::string, KeyValuePairList> SectionMap;

  bool Initialize(const char* config_file_path);
  bool Load();
  void Destroy();

  inline std::string& config_file_path() { return config_file_path_; }
  inline const std::string& config_file_path() const { return config_file_path_; }


  // 得到所有section的名字

  const char* GetString(const char* section, const char* key, const char* default_val=NULL) const;
  int GetInt(const char* section, const char* key, int default_val=0) const;
  CStringVector GetCStringList(const char* section, const char* key) const;
  StringVector GetStringList(const char* section, const char* key) const;

  // 取一section下所有的key
  StringVector GetSectionKey(const char *section) const;
  bool CheckBySection(const char* section) const;
  bool CheckBySectionAndKey(const char* section, const char* key) const;
  bool CheckByValue(const char* section, const char* key, const char* val) const;

  StringVector GetSectionName() const;

private:
  SectionMap config_map_;

  DISALLOW_COPY_AND_ASSIGN(ConfigFile);

  std::string config_file_path_;
};

}

#endif

