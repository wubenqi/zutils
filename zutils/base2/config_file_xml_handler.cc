// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "base2/config_file_xml_handler.h"

#include "base/strings/string_util.h"
#include "base2/xml/xml_attributes.h"


namespace {
const std::string kConfigXMLIniElement("XMLIni");
const std::string kConfigSectionElement("Section");
const std::string kConfigItemElement("Item");
const std::string kConfigNameAttribute("name");
const base::ConfigFile::KeyValuePairList kEmptyKeyValusePairList;
const std::string kEmptyString;
  
}

namespace base {

ConfigFileXmlHandler::ConfigFileXmlHandler(ConfigFile::SectionMap& config_map)
  : config_map_(config_map) {
}

bool ConfigFileXmlHandler::ElementStart(const std::string& element, const base::XMLAttributes& attributes) {
  // std::string t = kConfigNameAttribute;
  current_element_ = element;
  std::string attr_val;
  if (element==kConfigXMLIniElement) {
    // ¿ªÊ¼
    config_map_.clear();
  } else if (element==kConfigSectionElement) {
    if (!attributes.GetValue(kConfigNameAttribute, &attr_val)) {
      return false;
    }
    //LOG(INFO) << attr_val;
    //section_ = *attr_val;
    std::pair<ConfigFile::SectionMap::iterator, bool> result = config_map_.insert(std::make_pair(attr_val, kEmptyKeyValusePairList));
    if (result.second) {
    } else {
      LOG(ERROR) << "ConfigFileXmlHandler::ElementStart - config_map_.insert() error while parsing config file: '"
      << element
      << "' is unknown.";
    }
    current_it_ = result.first;
	} else if (element==kConfigItemElement) {
    if (!attributes.GetValue(kConfigNameAttribute, &attr_val)) {
      return false;
    }
    //LOG(INFO) << attr_val;
    current_it_->second.push_back(std::make_pair(attr_val, kEmptyString));
  } else {
    LOG(ERROR) << "ConfigFileXmlHandler::ElementStart - Unexpected data was found while parsing config file: '"
    << element
    << "' is unknown.";
    return false;
  }
	return true;
}

bool ConfigFileXmlHandler::ElementEnd(const std::string& element) {
  if (element==kConfigXMLIniElement) {
  } else if (element==kConfigSectionElement) {
    //section_.clear();
  } else if (element==kConfigItemElement) {
    //key_.clear();
    //value_.clear();
  } else {
    LOG(ERROR) << "ConfigFileXmlHandler::ElementStart - Unexpected data was found while parsing config file: '"
    << element
    << "' is unknown.";
    return false;
  }
  return true;
}

bool ConfigFileXmlHandler::Text(const std::string& text) {
  if (current_element_==kConfigItemElement) {
    TrimWhitespaceASCII(text, TRIM_ALL, &(current_it_->second.rbegin()->second));
  } else {
    LOG(ERROR) << "ConfigFileXmlHandler::ElementStart - Unexpected data was found while parsing config file: '"
    << current_element_
    << "' is unknown.";
    return false;
  }
	return true;
}
  
}


