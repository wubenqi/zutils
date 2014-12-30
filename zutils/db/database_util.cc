// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "db/database_util.h"

#include "base/logging.h"
#include "base/strings/stringprintf.h"

namespace db {

namespace {

void StringAppendQ(std::string* dest, const char* src, int count) {
	while (count-- > 0) {
		if (*src == '\'') {
			dest->push_back('\\');
		} else if (*src == '"') {
			dest->push_back('\\');
		} else if (*src == '\\') {
			dest->push_back('\\');
		}
		dest->push_back(*src++);
	}
}

}

bool MakeQueryString(const char *query, int lmt, std::string *query_string) {
	return MakeQueryString(query, NULL, lmt, query_string);
}

bool MakeQueryString(const char *query, QueryParam *param, std::string *query_string) {
	return MakeQueryString(query, param, 0, query_string);
}

bool MakeQueryString(const char *query, QueryParam *param, int lmt, std::string *query_string) {
	if (query==NULL || query_string==NULL) {
		LOG(ERROR) << "MakeQueryString param (query or query_string) is null";
		return false;
	}
	
	query_string->clear();

	const char *sp, *pprev;
	uint32 i = 0;

	sp = pprev = query;

	if (!param) {
		*query_string = query;
	} else {
		while(*sp++) {
			if (*sp == '%' || *sp == ':') {
				const char *sp_t = sp++;
				// 为%s或者:数字
				while (*sp && ((*sp == 's') || (*sp >=0x30 && *sp<=0x39)))
					sp++;
				if (sp_t == sp)
					continue;
				if (i > (param->size() - 1)) {
					LOG(ERROR) << "Error in sql number of param = " << param->size() << ", query=" << query << " ";
					query_string->clear();
					return false;
				}
				query_string->append(pprev, sp_t-pprev);
				if ((*param)[i].param) {
					switch ((*param)[i].type) {
					case kColumnTypeStr2:
						query_string->push_back('\'');
						if ((*param)[i].len) {
							StringAppendQ(query_string, (*param)[i].param, (*param)[i].len << 1);
						}
						query_string->push_back('\'');
						break;
					case kColumnTypeStrEsc:
					case kColumnTypeBlob:
						query_string->push_back('\'');
						if ((*param)[i].len) {
							StringAppendQ(query_string, (*param)[i].param, (*param)[i].len);
						}
						query_string->push_back('\'');
						break;
					case kColumnTypeStr:
						if (*(sp_t + 1) != 's') query_string->push_back('\'');
						if ((*param)[i].len) {
							StringAppendQ(query_string, (*param)[i].param, (*param)[i].len);
						}
						if (*(sp_t + 1) != 's') query_string->push_back('\'');;
						break;
					case kColumnTypeInt32:
						base::StringAppendF(query_string, "%d", *((int*)(*param)[i].param));
						break;
					case kColumnTypeUInt32:
						base::StringAppendF(query_string, "%lu", *((uint32*)(*param)[i].param));
						break;
					case kColumnTypeFloat:
						base::StringAppendF(query_string, "%.3f", *((float*)(*param)[i].param));
						break;
					case kColumnTypeDouble:
						base::StringAppendF(query_string, "%.3f", *((double*)(*param)[i].param));
						break;
						}
				} else {
					query_string->append("NULL");
				}
				pprev = sp;
				i++;
			}
		}
		query_string->append(pprev);

		if (i != param->size()) {
			LOG(ERROR) << "Error in sql number of param =  " << param->size() << ", query=" << query;
			query_string->clear();
			return false;
		}
	}

	if (lmt) {
		base::StringAppendF(query_string, " LIMIT %lu", lmt);
	}
	
	return true;
}

}


