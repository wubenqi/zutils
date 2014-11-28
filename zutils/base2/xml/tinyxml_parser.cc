/**
 * tinyxml_parser.cpp
 * DESCRIPTION:
 *	参考CEGUI的部分代码
 *
 * Copyright (C) 2007, www.soucheng.com
 * By Wu Benqi<wubenqi@youcity.com>, 2008-12-16
 */


#include "base2/xml/tinyxml_parser.h"

#include "base/logging.h"
//#include "base/file_util.h"
#include "base/stl_util.h"
#include "base/file_util.h"
#include "base2/file_path_util.h"

//#include <stdio.h>
//#include <sstream>
//#include <fstream>

#include "base2/xml/tinyxml_parser.h"
#include "base2/xml/xml_handler.h"
#include "base2/xml/xml_attributes.h"

#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif

#include "base2/xml/third_party/tinyxml/tinyxml.h"

namespace base {
//////////////////////////////////////////////////////////////////////////

class TinyXMLDocument :
	public TiXmlDocument {
public:
	explicit TinyXMLDocument(XMLHandler& handler);
	~TinyXMLDocument(){}

	bool ParseXMLData(char* buf, bool b_free = false);

protected:
	bool ProcessElement(const TiXmlElement* element);

private:
	XMLHandler* d_handler;
};

TinyXMLDocument::TinyXMLDocument(XMLHandler& handler) {
	d_handler = &handler;
}

bool TinyXMLDocument::ParseXMLData(char* buf, bool b_free) {
	char* d_buf = buf;

	// Parse the document
	TiXmlDocument doc;
	if (!doc.Parse((const char*)d_buf)) {
		// throw exception
		if(b_free) delete [] d_buf; 
		
		LOG(ERROR) << "TinyXMLParser: an error occurred while "
			"parsing the XML document - check it for potential errors!.";
		return false;
	}

	const TiXmlElement* currElement = doc.RootElement();
	if (currElement) {
		if(!ProcessElement(currElement)) {
			LOG(ERROR) << "TinyXMLParser: an error occurred while process xml element";
			if(b_free) delete [] d_buf;
			return false;
		}
	} // if (currElement)

	// Free memory
	if(b_free) delete [] d_buf;
	return true;
}

bool TinyXMLDocument::ProcessElement(const TiXmlElement* element) {
	// build attributes block for the element
	XMLAttributes attrs;

	const TiXmlAttribute *currAttr = element->FirstAttribute();
	while (currAttr) {
		attrs.Add(currAttr->Name(), currAttr->Value());
		currAttr = currAttr->Next();
	}

	// start element
	if(!d_handler->ElementStart(element->Value(), attrs)) {
		return false;
	}

	// do children
	const TiXmlNode* childNode = element->FirstChild();
	while (childNode) {
		switch(childNode->Type())
		{
		case ELEMENT:
			ProcessElement(childNode->ToElement());
			break;
		case TEXT:
			if (childNode->ToText()->Value() != NULL)
				d_handler->Text(childNode->ToText()->Value());
			break;

			// Silently ignore unhandled node type
		};
		childNode = childNode->NextSibling();
	}

	// end element
	if(!d_handler->ElementEnd(element->Value())) {
		return false;
	}

	return true;
}
	
TinyxmlParser::TinyxmlParser(void) {
    // set ID string
	d_identifierString = "base::TinyXMLParser - Official tinyXML based parser module";
}

TinyxmlParser::~TinyxmlParser(void) {
}

bool TinyxmlParser::ParseXMLData(XMLHandler& handler, const char* xml_text, size_t xml_text_size) {
	TinyXMLDocument doc(handler);
	return doc.ParseXMLData(const_cast<char*>(xml_text));
}

bool TinyxmlParser::ParseXMLData(XMLHandler& handler, const std::string &xml_text) {
	TinyXMLDocument doc(handler);
	return doc.ParseXMLData(const_cast<char*>(xml_text.c_str()));
}

bool TinyxmlParser::ParseXMLFile(XMLHandler& handler, const std::string& filename) {
	std::string data;
  if(!file_util::ReadFileToString(file_path_util::FromStringHack(filename), &data)) {
		LOG(ERROR) << "TinyxmlParser::ParseXMLFile - Unable to open XMLFile";
		return false;
	} else {
    data.push_back('\n');
    data.push_back('\0');
  }
	TinyXMLDocument doc(handler);
	return doc.ParseXMLData(string_as_array(&data));
}

bool TinyxmlParser::InitialiseImpl(void) {
    return true;
}

void TinyxmlParser::CleanupImpl(void) {

}

}
