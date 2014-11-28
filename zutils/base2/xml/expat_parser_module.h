/**
 * expat_parser_module.h
 * DESCRIPTION:
 *	参考CEGUI的部分代码
 *
 * Copyright (C) 2007, www.soucheng.com
 * By Wu Benqi<wubenqi@youcity.com>, 2008-12-16
 */

#ifndef __EXPAT_PARSER_MODULE_H_
#define __EXPAT_PARSER_MODULE_H_

base::XMLParser* createParser(void);
void destroyParser(base::XMLParser* parser);

#endif
