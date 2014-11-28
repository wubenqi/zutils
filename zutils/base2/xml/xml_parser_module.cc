/**
 * expat_parser_module.cpp
 * DESCRIPTION:
 *	参考CEGUI的部分代码
 *
 * Copyright (C) 2007, www.soucheng.com
 * By Wu Benqi<wubenqi@youcity.com>, 2008-12-16
 */


//#define USE_EXPAT_PARSER
//#define USE_MARKUPSTL_PARSER

#include "base2/xml/xml_parser_module.h"

//#define USE_EXPAT_PARSER
#define USE_TINYXML_PARSER
//#define USE_RAPIDXML_XMLPARSER

#if defined(USE_EXPAT_PARSER)
#include "base2/xml/expat_parser.h"
#endif

#if defined(USE_TINYXML_PARSER)
#include "base2/xml/tinyxml_parser.h"
#endif

#if defined(USE_RAPIDXML_XMLPARSER)
#include "base2/xml/rapidxml_parser.h"
#endif

#if !defined(USE_MARKUPSTL_PARSER) && !defined(USE_TINYXML_PARSER) && !defined(USE_MARKUPSTL_PARSER) && !defined(USE_RAPIDXML_XMLPARSER)
#include "base2/xml/tinyxml_parser.h"
#endif

base::XMLParser* CreateParser(int xml_type) {
	base::XMLParser* xml_parser = NULL;
#if defined(USE_EXPAT_PARSER)
	if (xml_type==base::XMLParser::EXPAT_XMLPARSER) {
		xml_parser = new base::ExpatParser();
	}
#endif

#if defined(USE_TINYXML_PARSER)
	if (xml_type==base::XMLParser::TINYXML_XMLPARSER) {
		xml_parser = new base::TinyxmlParser();
	}
#endif

#if defined(USE_RAPIDXML_XMLPARSER)
	if (xml_type==base::XMLParser::RAPIDXML_XMLPARSER) {
		xml_parser = new base::RapidxmlParser();
	}
#endif
  
	if (!xml_parser) {
		xml_parser = new base::TinyxmlParser();
	}
	return xml_parser;
}

void DestroyParser(base::XMLParser* parser) {
    delete parser;
}
