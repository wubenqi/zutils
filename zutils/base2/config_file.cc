// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "base2/config_file.h"

#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/strings/string_number_conversions.h"

#include "base2/xml/xml_lite.h"
#include "base2/config_file_xml_handler.h"

namespace {
  
const char* GetValueByKey(const base::ConfigFile::KeyValuePairList& key_values, const char* key, const char* default_val=NULL) {
  const char* val = default_val;
  for (size_t i=0; i<key_values.size(); ++i) {
    if (key_values[i].first == key) {
      val = key_values[i].second.c_str();
    }
  }
  return val;
}

void GetValueListByKey(const base::ConfigFile::KeyValuePairList& key_values, const char* key, std::vector<const char*>* values) {
  for (size_t i=0; i<key_values.size(); ++i) {
    if (key_values[i].first == key) {
      values->push_back(key_values[i].second.c_str());
    }
  }
}

void GetValueListByKey(const base::ConfigFile::KeyValuePairList& key_values, const char* key, std::vector<std::string>* values) {
  for (size_t i=0; i<key_values.size(); ++i) {
    if (key_values[i].first == key) {
      values->push_back(key_values[i].second.c_str());
    }
  }
}
  
}

namespace base {
  
bool ConfigFile::Initialize(const char* config_file_path) {
  config_file_path_ = config_file_path;
  return Load();
}

bool ConfigFile::Load() {
  ConfigFileXmlHandler xml_handler(config_map_);
  base::XMLLite xml_lite;
  bool result = xml_lite.GetXmlParser()->ParseXMLFile(xml_handler, config_file_path_.c_str());
  if (!result) {
    Destroy();
  }
  return result;
}

void ConfigFile::Destroy() {
  config_map_.clear();
}

const char* ConfigFile::GetString(const char* section, const char* key, const char* default_val) const {
  const char* val = default_val;
  SectionMap::const_iterator it = config_map_.find(section);
  if (it != config_map_.end()) {
    val = GetValueByKey(it->second, key, default_val);
  }
  return val;
}

int ConfigFile::GetInt(const char* section, const char* key, int default_val) const {
  int val = default_val;
  const char* str_val = GetString(section, key, NULL);
  if (str_val!=NULL) {
    base::StringToInt(str_val, &val);
  }
  return val;
}

CStringVector ConfigFile::GetCStringList(const char* section, const char* key) const {
  CStringVector values;
  SectionMap::const_iterator it = config_map_.find(section);
  if (it != config_map_.end()) {
    GetValueListByKey(it->second, key, &(values.cstring_vector));
  }
  return values;
}

StringVector ConfigFile::GetStringList(const char* section, const char* key) const {
  StringVector values;
  SectionMap::const_iterator it = config_map_.find(section);
  if (it != config_map_.end()) {
    GetValueListByKey(it->second, key, &(values.string_vector));
  }
  return values;
}

StringVector ConfigFile::GetSectionKey(const char *section) const {
  StringVector keys;
  SectionMap::const_iterator it = config_map_.find(section);
  if (it != config_map_.end()) {
    for (size_t i=0; i<it->second.size(); ++i) {
      keys.string_vector.push_back(it->second[i].first);
    }
  }
  return keys;
}

bool ConfigFile::CheckBySection(const char* section) const {
  return config_map_.find(section) != config_map_.end();
}

bool ConfigFile::CheckBySectionAndKey(const char* section, const char* key) const {
  bool result = false;
  SectionMap::const_iterator it = config_map_.find(section);
  if (it != config_map_.end()) {
    for (size_t i=0; i<it->second.size(); ++i) {
      if (it->second[i].first == key) {
        result = true;
        break;
      }
    }
  }
  return result;
}

bool ConfigFile::CheckByValue(const char* section, const char* key, const char* val) const {
  bool result = false;
  SectionMap::const_iterator it = config_map_.find(section);
  if (it != config_map_.end()) {
    for (size_t i=0; i<it->second.size(); ++i) {
      if (it->second[i].first == key && it->second[i].second == val) {
        result = true;
        break;
      }
    }
  }
  return result;
}

base::StringVector ConfigFile::GetSectionName() const {
  StringVector keys;
  SectionMap::const_iterator it;
  for (it=config_map_.begin(); it!=config_map_.end(); ++it) {
    keys.string_vector.push_back(it->first);
  }
  return keys;
}

}


