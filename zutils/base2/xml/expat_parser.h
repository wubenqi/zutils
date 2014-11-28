/**
 * expat_parser.h
 * DESCRIPTION:
 *	参考CEGUI的部分代码
 *
 * Copyright (C) 2007, www.soucheng.com
 * By Wu Benqi<wubenqi@youcity.com>, 2008-12-16
 */

#ifndef BASE_XML_XML_EXPAT_PARSER_H_
#define BASE_XML_XML_EXPAT_PARSER_H_

#include "base/xml/xml_parser.h"

namespace base {

class ExpatParser : 
	public XMLParser {
public:
	ExpatParser(void);
	virtual ~ExpatParser(void);
	// Implementation of public abstract interface
    
  virtual bool ParseXMLData(XMLHandler& handler, const std::string &xml_text);
  virtual bool ParseXMLData(XMLHandler& handler, const char* xml_text, size_t xml_text_size);
  virtual bool ParseXMLFile(XMLHandler& handler, const std::string& filename);

protected:
	// Implementation of protected abstract interface.
	virtual bool InitialiseImpl(void);
	// Implementation of protected abstract interface.
	virtual void CleanupImpl(void);
    
	// C++ class methods name are not valide C function pointer. static solve this
	static void StartElement(void* data, const char* element, const char**attr); // Expat handlers
	static void EndElement(void* data, const char* element); // Expat handlers
	static void CharacterData(void* data, const char* text, int len); // Expat handlers
};

}

#endif // BASE_XML_XML_EXPAT_PARSER_H_

