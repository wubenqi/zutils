/**
 * xml_attributes.h
 * DESCRIPTION:
 *	参考CEGUI的部分代码
 *
 * Copyright (C) 2008, www.soucheng.com
 * By Wu Benqi<wubenqi@youcity.com>, 2008-12-16
 */

#ifndef BASE2_XML_XML_ATTRIBUTES_H_
#define BASE2_XML_XML_ATTRIBUTES_H_
#pragma once

#include <string>
#include <map>
#include <set>

#include "base/basictypes.h"
#include "base/strings/string_piece.h"

namespace base {

class XMLAttributes {
public:
	XMLAttributes(void) {}
	virtual ~XMLAttributes(void) {}

	void Add(const StringPiece& attr_name, const StringPiece& attr_val);
	void Remove(const StringPiece& attr_name);
	bool Exists(const StringPiece& attr_name) const;
	size_t GetCount(void) const;
	
	bool GetName(size_t index, std::string* val) const;
  std::string GetName(size_t index) const;
	bool GetValue(size_t index, std::string* val) const;
  
	bool GetValue(const StringPiece& attr_name, std::string* val) const;
  bool GetValueAsBool(const StringPiece& attr_name, bool* val) const;
  bool GetValueAsInt(const StringPiece& attr_name, int* val) const;
  bool GetValueAsInt64(const StringPiece& attr_name, int64* val) const;
  bool GetValueAsFloat(const StringPiece& attr_name, float* val) const;

  void ToNames(std::set<StringPiece>* names) const;
  
  inline const std::map<StringPiece, StringPiece>& GetAttributes() const {
    return attrs_;
  }
  
protected:
	typedef std::map<StringPiece, StringPiece> AttributeMap;
	AttributeMap  attrs_;
};

}

#endif
