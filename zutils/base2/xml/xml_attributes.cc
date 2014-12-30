/**
 * xml_attributes.cpp
 * DESCRIPTION:
 *	≤ŒøºCEGUIµƒ≤ø∑÷¥˙¬Î
 *
 * Copyright (C) 2008, www.soucheng.com
 * By Wu Benqi<wubenqi@youcity.com>, 2008-12-16
 */

//#include "exception.h"
#include "base2/xml/xml_attributes.h"

#include <sstream>
#include <iterator>

#include "base/strings/string_util.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"


namespace base{

void XMLAttributes::Add(const StringPiece& attr_name, const StringPiece& attr_value) {
	attrs_[attr_name] = attr_value;
}

void XMLAttributes::Remove(const StringPiece& attr_name) {
	AttributeMap::iterator pos = attrs_.find(attr_name);

	if (pos != attrs_.end()) {
		attrs_.erase(pos);
	}
}

bool XMLAttributes::Exists(const StringPiece& attr_name) const {
	return attrs_.find(attr_name) != attrs_.end();
}

size_t XMLAttributes::GetCount(void) const {
	return attrs_.size();
}

bool XMLAttributes::GetName(size_t index, std::string* val) const {
	if (index >= attrs_.size()) {
		//LOG(ERROR) << "XMLAttributes::GetName - The specified index is out of range for this XMLAttributes block.";
		return false;
	}

	AttributeMap::const_iterator iter = attrs_.begin();
	std::advance(iter, index);
	*val = iter->first.as_string();
	return true;
}

std::string XMLAttributes::GetName(size_t index) const {
 	if (index >= attrs_.size()) {
		//LOG(ERROR) << "XMLAttributes::GetName - The specified index is out of range for this XMLAttributes block.";
		return "";
	}
  
	AttributeMap::const_iterator iter = attrs_.begin();
	std::advance(iter, index);
	return iter->first.as_string();
}
  
bool XMLAttributes::GetValue(size_t index, std::string* val) const {
	if (index >= attrs_.size()) {
		//LOG(ERROR) << "XMLAttributes::GetValue - The specified index is out of range for this XMLAttributes block.";
		return false;
	}

	AttributeMap::const_iterator iter = attrs_.begin();
	std::advance(iter, index);

	*val = iter->second.as_string();
	return true;
}

bool XMLAttributes::GetValue(const StringPiece& attr_name, std::string* val) const {
	AttributeMap::const_iterator pos = attrs_.find(attr_name);

	if (pos != attrs_.end()) {
    *val = pos->second.as_string();
		return true;
	} else {
		//LOG(ERROR) << "XMLAttributes::getValue - no value exists for an attribute named '" << attr_name << "'.";
		return false;
	}
}

bool XMLAttributes::GetValueAsBool(const StringPiece& attr_name, bool* val) const {
	AttributeMap::const_iterator pos = attrs_.find(attr_name);
  
	if (pos == attrs_.end()) {
 		//LOG(ERROR) << "XMLAttributes::GetValueAsBool - no value exists for an attribute named '" << attr_name << "'.";
		return false;
	}

	if (pos->second == "false" || pos->second == "0") {
		*val = false;
		return true;
	} else if (pos->second == "true" || pos->second == "1") {
		*val = true;
		return true;
	} else {
		LOG(ERROR) << "XMLAttributes::getValueAsInteger - failed to convert attribute '" << attr_name <<  "' with value '" << pos->second << "' to bool.";
		//d_error = -3;
		return false;
	}
}

bool XMLAttributes::GetValueAsInt(const StringPiece& attr_name, int* val) const {
	AttributeMap::const_iterator pos = attrs_.find(attr_name);
  
	if (pos == attrs_.end()) {
 		//LOG(ERROR) << "XMLAttributes::GetValueAsInt - no value exists for an attribute named '" << attr_name << "'.";
		return false;
	}

  if (!base::StringToInt(pos->second, val)) {
    LOG(ERROR) << "XMLAttributes::GetValueAsInt - failed to convert attribute '" << attr_name <<  "' with value '" << pos->second << "' to integer.";
    return false;
  }
	return true;
}

bool XMLAttributes::GetValueAsInt64(const StringPiece& attr_name, int64* val) const {
  AttributeMap::const_iterator pos = attrs_.find(attr_name);
  
  if (pos == attrs_.end()) {
    //LOG(ERROR) << "XMLAttributes::GetValueAsInt64 - no value exists for an attribute named '" << attr_name << "'.";
    return false;
  }
  
  if (!base::StringToInt64(pos->second, val)) {
    LOG(ERROR) << "XMLAttributes::GetValueAsInt64 - failed to convert attribute '" << attr_name <<  "' with value '" << pos->second << "' to integer.";
    return false;
  }
  return true;
}
  
bool XMLAttributes::GetValueAsFloat(const StringPiece& attr_name, float* val) const {
	AttributeMap::const_iterator pos = attrs_.find(attr_name);
  
	if (pos == attrs_.end()) {
 		//LOG(ERROR) << "XMLAttributes::GetValueAsFloat - no value exists for an attribute named '" << attr_name << "'.";
		return false;
	}
  
  double tmp_val;
  
  if (!base::StringToDouble(pos->second.as_string(), &tmp_val)) {
    LOG(ERROR) << "XMLAttributes::GetValueAsFloat - failed to convert attribute '" << attr_name <<  "' with value '" << pos->second << "' to integer.";
    return false;
  }
  
  *val = static_cast<float>(tmp_val);
	return true;
}
  
void XMLAttributes::ToNames(std::set<StringPiece>* names) const {
  AttributeMap::const_iterator it = attrs_.begin();
  for (; it!=attrs_.end(); ++it) {
    names->insert(it->first);
  }
}


} // End of  base namespace section

