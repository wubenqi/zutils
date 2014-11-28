/**
 * xml_parser.cpp
 * DESCRIPTION:
 *	参考CEGUI的部分代码
 *
 * Copyright (C) 2007, www.soucheng.com
 * By Wu Benqi<wubenqi@youcity.com>, 2008-12-16
 */

#include "base2/xml/xml_parser.h"

namespace base {

//const std::string XMLParser::EXPAT_XMLPARSER("expat");
//const std::string XMLParser::TINYXML_XMLPARSER("tinyxml");
//const std::string XMLParser::MARKUPSTL_XMLPARSER("markupstl");

XMLParser::XMLParser(void) :
	d_identifierString("Unknown XML parser (vendor did not set the ID string!)"),
	d_initialised(false) {

}

XMLParser::~XMLParser(void){

}

bool XMLParser::Initialise(void) {
	// do this to ensure only one initialise call is made
	if (!d_initialised) {
		d_initialised = InitialiseImpl();
	}

	return d_initialised;
}

void XMLParser::Cleanup(void) {
	if (d_initialised) {
		CleanupImpl();
		d_initialised = false;
	}
}

const std::string& XMLParser::GetIdentifierString() const {
	return d_identifierString;
}

} // End of  base namespace section
