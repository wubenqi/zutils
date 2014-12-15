// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef DB_SQLITE_SQLITE_DATABASE_H_
#define DB_SQLITE_SQLITE_DATABASE_H_
#pragma once


#include "db/base_database.h"
#include "sql/connection.h"

namespace db {

class SQLiteDatabase :
	public virtual BaseDatabase {
public:
	SQLiteDatabase();
	virtual ~SQLiteDatabase();

	// 查询语句
	virtual QueryAnswer *Query(const char *q_str);
	virtual QueryAnswer *Query(const char *q_str, size_t len);
	virtual QueryAnswer *Query(const char *q_str, size_t len, int* err);

	// 插入
	// 返回INSERT 操作产生的 ID
	virtual uint64 ExecuteInsertID(const char *q_str);
	virtual uint64 ExecuteInsertID(const char *q_str, size_t len);
	virtual uint64 ExecuteInsertID(const char *q_str, size_t len, int* err);

	// 插入和更新
	virtual int Execute(const char *q_str);
	virtual int Execute(const char *q_str, size_t len);
	virtual int Execute(const char *q_str, size_t len, int* err);

	virtual bool BeginTransaction();
	virtual bool CommitTransaction();
	virtual bool RollbackTransaction();

	virtual bool Open(const char* host, const char* user, const char* password, const char* dbname, int port);

	virtual void CloseDb();
	virtual int GetError(int err);
	virtual std::string DispError();

protected:
	virtual bool Ping();

private:
	sql::Connection db_;
};

}

#endif

