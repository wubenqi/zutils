// Copyright 2010, www.lelewan.com
// All rights reserved
//
// Author: wubenqi<wubenqi@caitong.net>, 2010-10-15
//

#include "db/sqlite/sqlite_database.h"

#include "base/logging.h"
#include "sql/statement.h"

namespace db {

enum DBError {
	kDBErrorUnknown = 0,
	kDBErrorDupEntry,
	kDBErrorMYSQLNotInited,
	kDBErrorQueryIsNull,
};

class SQLiteAnswer
	: public QueryAnswer {
public:
	SQLiteAnswer(sql::Statement* s, uint32 columns)
		: QueryAnswer() {
		stmt_ = s;
		column_count_ = columns;
	}
	virtual ~SQLiteAnswer() {
		delete stmt_;
		stmt_ = NULL;
	}

	virtual bool FetchRow() {
		return stmt_->Step();
	}

	virtual bool		GetColumnBool(int col) const {
		return stmt_->ColumnBool(col);
	}

	virtual int			GetColumnInt(int col) const {
		return stmt_->ColumnInt(col);
	}

	virtual int64		GetColumnInt64(int col) const {
		return stmt_->ColumnInt64(col);
	}

	virtual double		GetColumnDouble(int col) const {
		return stmt_->ColumnDouble(col);
	}

	virtual std::string	GetColumnString(int col) const {
		return stmt_->ColumnString(col);
	}


#if 0
	virtual uint32 GetColumnLength(uint32 clm) const {
		CHECK(lengths_);
		CHECK(res_);
		return static_cast<uint32>(lengths_[clm]);
	}


	virtual bool GetColumn(uint32 clm, uint64* val) const {
		if (row_[clm]) {
			int64 val2;
			bool ret = base::StringToInt64(row_[clm], row_[clm]+GetColumnLength(clm), &val2);
			if (ret) *val = (uint64)(val2);
			return ret;
		} else {
			*val = 0;
			return false;
		}
	}

	virtual bool GetColumn(uint32 clm, int64* val) const {
		if (row_[clm]) {
			return base::StringToInt64(row_[clm], row_[clm]+GetColumnLength(clm), val);
		} else {
			*val = 0;
			return false;
		}
	}

	virtual bool GetColumn(uint32 clm, uint32 *val) const {
		if (row_[clm]) {
			*val = strtoul(row_[clm], NULL, 10);
			return true;
		} else {
			*val = 0;
			return false;
		}
	}

	virtual bool GetColumn(uint32 clm, int *val) const {
		if (row_[clm]) {
			*val = atoi(row_[clm]);
			return true;
		} else {
			*val = 0;
			return false;
		}
	}

	virtual bool GetColumn(uint32 clm, float* val) const {
		if (row_[clm]) {
			double val2;
			// TODO 多此一举
			std::string s_clm(row_[clm], GetColumnLength(clm));
			bool ret = base::StringToDouble(s_clm, &val2);
			if (ret) *val = (float)(val2);
			return ret;
		} else {
			*val = 0.0;
			return false;
		}
	}

	virtual bool GetColumn(uint32 clm, bool* val) const {
		if (row_[clm]) {
			int val2;
			bool ret = base::StringToInt(row_[clm], row_[clm]+GetColumnLength(clm), &val2);
			if (ret) *val = (val2==0 ? false:true);
			return ret;
		} else {
			*val = false;
			return false;
		}
	}
#endif

protected:
	sql::Statement* stmt_;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
SQLiteDatabase::SQLiteDatabase() {
}

SQLiteDatabase::~SQLiteDatabase() {
	CloseDb();
}

QueryAnswer *SQLiteDatabase::Query(const char *q_str) {
	sql::Statement* s = new sql::Statement(db_.GetCachedStatement(SQL_FROM_HERE, q_str));
	SQLiteAnswer *answ = NULL;

	if (s) {
		if(s->is_valid()) {
			if (s->Step()) {
				answ = new SQLiteAnswer(s, s->ColumnCount());
			} else {
				delete s;
			}
		} else {
			delete s;
		}
	}

	return answ;
}

QueryAnswer *SQLiteDatabase::Query(const char *q_str, size_t len) {
	NOTREACHED() << "SQLiteDatabase's Query(const char *q_str, size_t len) not reached";
	return 0;
}

QueryAnswer *SQLiteDatabase::Query(const char *q_str, size_t len, int* err) {
	NOTREACHED() << "SQLiteDatabase's Query(const char *q_str, size_t len, int* err) not reached";
	return 0;
}

uint64 SQLiteDatabase::ExecuteInsertID(const char *q_str) {
	if(db_.Execute(q_str)) {
		return db_.GetLastInsertRowId();
	}
	return 0;
}

uint64 SQLiteDatabase::ExecuteInsertID(const char *q_str, size_t len) {
	NOTREACHED() << "SQLiteDatabase's ExecuteInsertID(const char *q_str, size_t len) not reached";
	return 0;
}

uint64 SQLiteDatabase::ExecuteInsertID(const char *q_str, size_t len, int* err) {
	NOTREACHED() << "SQLiteDatabase's ExecuteInsertID(const char *q_str, size_t len, int* err) not reached";
	return 0;
}

int SQLiteDatabase::Execute(const char *q_str) {
	if(db_.Execute(q_str)) {
		return 0;
	}
	return db_.GetLastErrno();
}

int SQLiteDatabase::Execute(const char *q_str, size_t len) {
	NOTREACHED() << "SQLiteDatabase's Execute(const char *q_str, size_t len) not reached";
	return 0;
}

int SQLiteDatabase::Execute(const char *q_str, size_t len, int* err) {
	NOTREACHED() << "SQLiteDatabase's Execute(const char *q_str, size_t len, int* err) not reached";
	return 0;
}

namespace {

const char kBeginTransaction[] = "START TRANSACTION";
const char kCommitTransaction[] = "COMMIT";
const char kRollbackTransaction[] = "ROLLBACK";

const char kSetNamesUTF8[] = "SET NAMES `utf8`";
const char kSetCharactesUTF8[] = "SET CHARACTER SET `utf8`";

}

bool SQLiteDatabase::BeginTransaction() {
	return 0==BaseDatabase::Execute(kBeginTransaction);
}

bool SQLiteDatabase::CommitTransaction() {
	return 0==BaseDatabase::Execute(kCommitTransaction);
}

bool SQLiteDatabase::RollbackTransaction() {
	int er = 0;
	return 0==BaseDatabase::Execute(kRollbackTransaction);
}

bool SQLiteDatabase::Open(const char* host, const char* user, const char* password, const char* dbname, int port) {
	return db_.Open(dbname);
}

int SQLiteDatabase::GetError(int err) {
	return kDBErrorUnknown;
}

std::string SQLiteDatabase::DispError(void) {
	return db_.GetErrorMessage();
}

void SQLiteDatabase::CloseDb() {
	db_.Close();
}

bool SQLiteDatabase::Ping() {
	return true;
}

}

//// Functions for use after dlopen()ing
//SQLDatabasePtr NewDataBase(void) {
//	return new db::SQLiteDatabase;
//}
