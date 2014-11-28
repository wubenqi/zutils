/**
 * expat_parser.cpp
 * DESCRIPTION:
 *	参考CEGUI的部分代码
 *
 * Copyright (C) 2007, www.soucheng.com
 * By Wu Benqi<wubenqi@youcity.com>, 2008-12-16
 */

#include "base/xml/expat_parser.h"

#include <expat.h>

#include "base/string_piece.h"
#include "base/sys_string_conversions.h"
#include "base/logging.h"
#include "base/file_util.h"
#include "base/xml/xml_handler.h"
#include "base/xml/xml_attributes.h"
//#include "exception.h"

// #include <sstream>

namespace base {

ExpatParser::ExpatParser(void) {
    // set ID string
    d_identifierString = "base::ExpatParser - Official expat based parser module!!!";
}

ExpatParser::~ExpatParser(void) {
}

bool ExpatParser::ParseXMLData(XMLHandler& handler, const char* xml_text, size_t xml_text_size) {
	// All stuff goes here
	XML_Parser parser = XML_ParserCreate(0); // Create a parser

	if (! parser) {
		LOG(ERROR) << "ExpatParser::parseXMLFile - Unable to create a new Expat Parser";
	}

	XML_SetUserData(parser, (void*)&handler); // Initialise user data
	XML_SetElementHandler(parser, StartElement, EndElement); // Register callback for elements
	XML_SetCharacterDataHandler(parser, CharacterData); // Register callback for character data

	// Parse the data (note that the last true parameter tels Expat that this is the last chunk of the document
	if ( ! XML_Parse(parser, xml_text, static_cast<int>(xml_text_size), true)) {
		LOG(ERROR) << "ExpatParser::parseXMLFile - XML Parsing error '"
			<< XML_ErrorString(XML_GetErrorCode(parser))
			<< "' at line "
			<< XML_GetCurrentLineNumber(parser);
		// (We know it is a valid pointer, otherwise an exception would have been thrown above.)
		XML_ParserFree(parser);
    return false;
	}

	// (We know it is a valid pointer, otherwise an exception would have been thrown above.)
	XML_ParserFree(parser);
  return true;
}

bool ExpatParser::ParseXMLData(XMLHandler& handler, const std::string &xml_text) {
	return ParseXMLData(handler, xml_text.c_str(), xml_text.length());
}

bool ExpatParser::ParseXMLFile(XMLHandler& handler, const std::string& filename) {
  std::string data;
  if(!ReadFileToString(filename.c_str(), &data)) {
    LOG(ERROR) << "TinyxmlParser::ParseXMLFile - Unable to open XMLFile";
    return false;
  }
  
  //TinyXMLDocument doc(handler);
  //return doc.ParseXMLData(string_as_array(&data));
  return ParseXMLData(handler, data.c_str(), data.length());
}
  
bool ExpatParser::InitialiseImpl(void) {
    return true;
}

void ExpatParser::CleanupImpl(void) {

}

void ExpatParser::StartElement(void* data, const char* element, const char** attr) {
    XMLHandler* handler = static_cast<XMLHandler*>(data);
    XMLAttributes attrs;

	for(size_t i = 0 ; attr[i] ; i += 2) {
        attrs.Add(attr[i], attr[i+1]);
	}

    handler->ElementStart(element, attrs);
}

void ExpatParser::EndElement(void* data, const char* element) {
    XMLHandler* handler = static_cast<XMLHandler*>(data);
    handler->ElementEnd(element);
}

void ExpatParser::CharacterData(void *data, const char *text, int len) {
    XMLHandler* handler = static_cast<XMLHandler*>(data);
    std::string str(text, static_cast<std::string::size_type>(len));
    handler->Text(str);
}

}
