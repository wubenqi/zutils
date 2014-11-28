/**
 * xml_lite.cc
 * DESCRIPTION:
 *
 * Copyright (C) 2007, www.soucheng.com
 * By Wu Benqi<wubenqi@youcity.com>, 2008-12-28
 */

#include "base2/xml/xml_lite.h"

#include "base2/xml/xml_parser_module.h"

namespace base {

XMLLite::XMLLite(int xml_type) {
	xml_parser_ = ::CreateParser(xml_type);
	xml_parser_->Initialise();

}

XMLLite::~XMLLite() {
	if (xml_parser_) {
		xml_parser_->Cleanup();
		::DestroyParser(xml_parser_);
	}
}

}
