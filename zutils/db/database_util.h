// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef DB_DATABASE_UTIL_H_
#define DB_DATABASE_UTIL_H_
#pragma once

#include <string>
#include <vector>
#include "base/basictypes.h"

namespace db {

enum ColumnType { 
	kColumnTypeStrEsc = 0,
	kColumnTypeStr,
	kColumnTypeInt32,
	kColumnTypeUInt32,
	kColumnTypeBlob,
	kColumnTypeStr2,
	kColumnTypeFloat,
	kColumnTypeDouble
};

struct QueryParamItem {
	int type;
	int len;
	const char *param;
};

class QueryParam : 
	public std::vector<QueryParamItem> {
public:

	void AddParamEsc(const char *param) {
		_addparam(param, static_cast<int>(strlen(param)), kColumnTypeStrEsc);
	}

	void AddParam(const char *param) {
		_addparam(param, static_cast<int>(strlen(param)), kColumnTypeStr);
	}

	void AddParam(const int *param) {
		_addparam((const char*)param, sizeof(int), kColumnTypeInt32 );
	}

	void AddParam(const float *param) {
		_addparam((const char*)param, sizeof(float), kColumnTypeFloat );
	}

	void AddParam(const uint32 *param) {
		_addparam((const char*)param, sizeof(uint32), kColumnTypeUInt32);
	}

	void AddParam(const char *param, int len) {
		_addparam((const char*)param, len, kColumnTypeStr);
	}

	void AddParamEsc(const char *param, int len) {
		_addparam(param, len, kColumnTypeBlob);
	}

	void AddParam(const double *param) {
		_addparam((const char*)param, sizeof(double), kColumnTypeDouble);
	}

	void Clear() {
		clear();
	}

private:
	void _addparam(const char *param, int len, int type) {
		QueryParamItem p;
		p.len = len;
		p.type = type;
		p.param = param;
		push_back(p);
	}
};


bool MakeQueryString(const char *query, int lmt, std::string *query_string);
bool MakeQueryString(const char *query, QueryParam *param, std::string *query_string);
bool MakeQueryString(const char *query, QueryParam *param, int lmt, std::string *query_string);

}

#endif
