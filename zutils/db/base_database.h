// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef DB_BASE_DATABASE_H_
#define DB_BASE_DATABASE_H_

#include "base/basictypes.h"

#include "base/strings/string_piece.h"
#include "base2/config_file_util.h"

namespace db {
  
class QueryAnswer {
public:
  QueryAnswer() :
  column_count_(0),
  row_count_(0) {
  }
  
  virtual ~QueryAnswer() {}
  
  //------------------------------------------------------------------------
  virtual bool FetchRow() = 0;
  virtual void PrintDebugFieldNames() const = 0;
  
  virtual const char* GetColumnName(uint32 clm) const = 0;

  std::string GetColumnValue(uint32 clm) const {
    std::string ret;
    GetColumn(clm, &ret);
    return ret;
  }

  std::string GetColumnValue(const base::StringPiece& clm) const {
    std::string ret;
    GetColumn(clm, &ret);
    return ret;
  }

  //------------------------------------------------------------------------
  virtual bool GetColumn(uint32 clm, std::string* val) const = 0;
  virtual uint32 GetColumnLength(uint32 clm) const = 0;
  virtual bool ColumnIsNull(uint32 clm) const = 0;
  
  virtual bool GetColumn(uint32 clm, uint64* val) const = 0;
  virtual bool GetColumn(uint32 clm, int64* val) const = 0;
  virtual bool GetColumn(uint32 clm, uint32* val) const = 0;
  virtual bool GetColumn(uint32 clm, int* val) const = 0;
  virtual bool GetColumn(uint32 clm, float* val) const = 0;
  virtual bool GetColumn(uint32 clm, bool* val) const = 0;
  
  //------------------------------------------------------------------------
  virtual bool GetColumn(const base::StringPiece& clm, std::string* val) const {
    uint32 index = GetIndexByFieldName(clm);
    if (index < column_count_) {
      return GetColumn(index, val);
    } else {
      return false;
    }
  }
  virtual uint32 GetColumnLength(const base::StringPiece& clm) const {
    uint32 index = GetIndexByFieldName(clm);
    if (index < column_count_) {
      return GetColumnLength(index);
    } else {
      return 0;
    }
  }
  
  virtual bool ColumnIsNull(const base::StringPiece& clm) const {
    bool result = false;
    uint32 index = GetIndexByFieldName(clm);
    if (index < column_count_) {
      result = ColumnIsNull(index);
    }
    return result;
  }
  
  virtual bool GetColumn(const base::StringPiece& clm, uint64* val) const {
    bool result = false;
    uint32 index = GetIndexByFieldName(clm);
    if (index < column_count_) {
      result = GetColumn(index, val);
    }
    return result;
  }
  
  virtual bool GetColumn(const base::StringPiece& clm, int64* val) const {
    bool result = false;
    uint32 index = GetIndexByFieldName(clm);
    if (index < column_count_) {
      result = GetColumn(index, val);
    }
    return result;
  }
  
  virtual bool GetColumn(const base::StringPiece& clm, uint32* val) const {
    bool result = false;
    uint32 index = GetIndexByFieldName(clm);
    if (index < column_count_) {
      result = GetColumn(index, val);
    }
    return result;
  }
  
  virtual bool GetColumn(const base::StringPiece& clm, int* val) const {
    bool result = false;
    uint32 index = GetIndexByFieldName(clm);
    if (index < column_count_) {
      result = GetColumn(index, val);
    }
    return result;
  }
  
  virtual bool GetColumn(const base::StringPiece& clm, float* val) const {
    bool result = false;
    uint32 index = GetIndexByFieldName(clm);
    if (index < column_count_) {
      result = GetColumn(index, val);
    }
    return result;
  }
  
  virtual bool GetColumn(const base::StringPiece& clm, bool* val) const {
    bool result = false;
    uint32 index = GetIndexByFieldName(clm);
    if (index < column_count_) {
      result = GetColumn(index, val);
    }
    return result;
  }
  
  //------------------------------------------------------------------------
  inline uint32 GetColumnCount() const { return column_count_; }
  inline uint64 GetRowCount() const { return row_count_; }
  
protected:
  virtual uint32 GetIndexByFieldName(const base::StringPiece& name) const {
    return column_count_;
  }
  
  uint32 column_count_;
  uint64 row_count_;
};
  
//------------------------------------------------------------------------
  
// 数据库操作有错误，打印在日志里
class BaseDatabase {
public:
//  enum DBError {
//    kDBErrorUnknown = 0,
//    kDBErrorDupEntry,
//    kDBErrorNotInited,
//    kDBErrorQueryIsNull,
//  };

  //static const base::StringPiece kEmptyStringPiece;
  BaseDatabase() :
    auto_commit_(true) {
  }
  
  virtual ~BaseDatabase() {
    CloseDb();
  }
  
  virtual base::StringPiece GetDatabaseName() const = 0;
  
  //virtual bool Open(const base::StringPiece& conn_string);
  virtual bool Open(const base::DBAddrInfo& db_addr, bool auto_commit = true) {
    auto_commit_ = auto_commit;
    db_addr_ = db_addr;
    return BuildConnection();
  }
  
  virtual void CloseDb(){}
  
  // 查询语句
  // 返回值如果为NULL,有两种情况:
  //  1. 无记录
  //  2. 执行查询时发生了错误
  // 如果为NULL，我们可以调用GetLastError()来检查是否是无记录还是发生了其它错误
  virtual QueryAnswer* Query(const base::StringPiece& q_str) = 0;
  
  // 插入
  // 返回INSERT 操作产生的 ID
  // 如果返回值为0,出错
  virtual uint64 ExecuteInsertID(const base::StringPiece& q_str) = 0;
  
  // 插入，更新和删除
  // 返回值为>=0，成功，受影响的行数
  // <0 出错
  virtual int Execute(const base::StringPiece& q_str) = 0;
  
  virtual bool BeginTransaction() { return false; }
  virtual bool CommitTransaction() { return false; }
  virtual bool RollbackTransaction() { return false; }
  
  virtual bool AutoCommintOn() { return false; }
  virtual bool AutoCommintOff() { return false; }
  
  virtual int GetLastError() = 0;
  
  // 数据库自增ID
  virtual uint64 GetNextID(const char* table_name, const char* field_name=NULL) = 0;
  
protected:
  //virtual bool CheckConnection() = 0;
  virtual bool BuildConnection() = 0;
  virtual bool Ping() { return true; }
  
  base::DBAddrInfo db_addr_;
  bool auto_commit_;
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

#endif /* _SQLDB_H_ */

