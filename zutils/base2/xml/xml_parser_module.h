/**
 * xml_parser_module.h
 * DESCRIPTION:
 *	参考CEGUI的部分代码
 *
 * Copyright (C) 2007, www.soucheng.com
 * By Wu Benqi<wubenqi@youcity.com>, 2008-12-16
 */

#ifndef BASE2_XML_XML_PARSER_MODULE_H_
#define BASE2_XML_XML_PARSER_MODULE_H_
#pragma once

#include "base2/xml/xml_parser.h"

base::XMLParser* CreateParser(int xml_type);
void DestroyParser(base::XMLParser* parser);

#endif
