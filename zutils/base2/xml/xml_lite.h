/**
 * xml_lite.h
 * DESCRIPTION:
 *
 * Copyright (C) 2007, www.soucheng.com
 * By Wu Benqi<wubenqi@youcity.com>, 2008-12-16
 */

#ifndef BASE2_XML_XML_LITE_H_
#define BASE2_XML_XML_LITE_H_
#pragma once

#include <string>
#include "base/basictypes.h"
#include "base2/xml/xml_parser.h"

// #include "base/singleton.h"

namespace base {


//  };: public Singleton<XMLLite> {
class XMLLite {
public:
	explicit XMLLite(int xml_type=XMLParser::TINYXML_XMLPARSER);
	virtual ~XMLLite();

	XMLParser* GetXmlParser() {
		return xml_parser_;
	}

private:
	XMLParser* xml_parser_;
	DISALLOW_COPY_AND_ASSIGN(XMLLite);
};

}

//typedef Singleton<base::XMLLite>	XMLLiteSinleton;

#endif
