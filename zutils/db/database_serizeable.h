// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef DB_DATABASE_SERIALIZABLE_H_
#define DB_DATABASE_SERIALIZABLE_H_

#include "db/database_util.h"

namespace db {

class QueryParam;
class QueryAnswer;
class DatabaseSerializable {
public:
	virtual ~DatabaseSerializable() {}

	virtual bool ParseFromDatabase(const QueryAnswer& answ) { return true; }
  virtual bool SerializeToDatabase(QueryParam* param) const { return true; }
};


}

#define DB_GET_RETURN_COLUMN(in, out) \
	result = answ.GetColumn(in, &(out)); \
	if (!result) { \
		LOG(ERROR) << "Parse column " << #in << " data error."; \
		break; \
	}

#define DB_GET_COLUMN(in, out) \
	if (!answ.ColumnIsNull(in)) { \
		out = answ.GetColumn(in); \
	}

#endif
