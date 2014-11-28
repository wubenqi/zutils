// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "db/conn_pool_manager.h"

#include "base/stringprintf.h"

#ifdef USE_DATABASE_DLL
#include "base2/dyn_lib_manager.h"
#endif

#include "db/database_dll.h"

namespace db {

CdbConnPoolManager::~CdbConnPoolManager() {
  Shutdown();
}

size_t CdbConnPoolManager::Initialize(const base::DBAddrInfo& db_addr, bool auto_commit) {
  auto_commit_ = auto_commit;

  FN_CreateDatabaseObject fn_create_database_object = NULL;
#ifdef USE_DATABASE_DLL
  try {
    dyn_lib_ = base::DynLibManager::GetInstance()->load((db_addr.db_type+"_dll").c_str());
    //CHECK(dyn_lib_) << "Load dll " << db_addr.db_type << "_dll.dll error";
    void* fn = dyn_lib_->getSymbol("CreateDatabaseObject");
    //CHECK(fn) << "getSymbol CreateDatabaseObject error";
    
    fn_create_database_object = (FN_CreateDatabaseObject)fn;//dyn_lib_->getSymbol("LuabindPBModule_Register");
    //CHECK(fn_create_database_object) << "getSymbol CreateDatabaseObject error";
    // fn_luabind_pb_module_register(script_engine_.GetLuaState());
  } catch (std::exception& e) {
    LOG(ERROR) << e.what();
  } catch (...) {
    LOG(ERROR) << "Invalid error";
  }
#else
  fn_create_database_object = CreateDatabaseObject;
#endif

  db_addr_ = db_addr;
  //conn_string_ = conn_string;
  
  //min_conn_count_ = min_conn_count;
  //max_conn_count_ = max_conn_count;
  
  conn_pool_.resize(db_addr.max_conn_count);
  
  for (int i=0; i<db_addr.max_conn_count; ++i) {
    if (fn_create_database_object) {
      conn_pool_[i].state = DatabaseConnection::kDBConnState_Created;
      conn_pool_[i].conn = fn_create_database_object();
      //conn_pool_[i].conn = new MySQLDatabase();
      
      if (i<db_addr.min_conn_count) {
        if (conn_pool_[i].conn->Open(db_addr)) {
          conn_pool_[i].state = DatabaseConnection::kDBConnState_Idle;
        }
        conned_pointer_++;
      }
    }
  }
  
  
  return conned_pointer_;
}

void CdbConnPoolManager::Shutdown() {
  FN_DestroyDatabaseObject fn_destroy_database_object = NULL;
#ifdef USE_DATABASE_DLL
  if (dyn_lib_) {
    try {
      //dyn_lib_ = base::DynLibManager::GetInstance()->load((db_type_+"_dll").c_str());
      //CHECK(dyn_lib_) << "Load dll " << db_type_ << "_dll.dll error";
      void* fn = dyn_lib_->getSymbol("DestroyDatabaseObject");
      CHECK(fn) << "getSymbol DestroyDatabaseObject error";
      
      fn_destroy_database_object = (FN_DestroyDatabaseObject)fn;
      CHECK(fn_destroy_database_object) << "getSymbol DestroyDatabaseObject error";
      
    } catch (std::exception& e) {
      LOG(ERROR) << e.what();
    } catch (...) {
      LOG(ERROR) << "Invalid error";
    }
  }
#else
  fn_destroy_database_object = DestroyDatabaseObject;
#endif

  for (size_t i=0; i<conn_pool_.size(); ++i) {
    if (conn_pool_[i].conn != NULL) {
      if (fn_destroy_database_object) {
        fn_destroy_database_object(conn_pool_[i].conn);
      } else {
        delete conn_pool_[i].conn;
      }
    }
  }
  conn_pool_.clear();
  
#ifdef USE_DATABASE_DLL
  if (dyn_lib_!=NULL) {
    base::DynLibManager::GetInstance()->unload(dyn_lib_);
    dyn_lib_ = NULL;
  }
#endif
}

BaseDatabase* CdbConnPoolManager::GetFreeConnection() {
  BaseDatabase* conn = NULL;
  base::AutoLock auto_lock(mutex_);
  
  for (int i=0; i<conned_pointer_; ++i) {
    if (conn_pool_[i].state == DatabaseConnection::kDBConnState_Idle) {
      conn_pool_[i].state = DatabaseConnection::kDBConnState_Busy;
      conn = conn_pool_[i].conn;
      break;
    }
  }
  if (conn == NULL && conned_pointer_ < db_addr_.max_conn_count
      && conn_pool_[conned_pointer_].state==DatabaseConnection::kDBConnState_Created) {
    // Á¬½Óµ½db
    if (conn_pool_[conned_pointer_].conn->Open(db_addr_)) {
      conn_pool_[conned_pointer_].state = DatabaseConnection::kDBConnState_Busy;
      conn = conn_pool_[conned_pointer_].conn;
      conned_pointer_++;
    }
  }
  return conn;
}

void CdbConnPoolManager::SetFreeConnection(BaseDatabase* conn) {
  base::AutoLock lock(mutex_);
  for (int i=0; i<conned_pointer_; ++i) {
    if (conn_pool_[i].conn == conn) {
      conn_pool_[i].state = DatabaseConnection::kDBConnState_Idle;
      break;
    }
  }
}

}
