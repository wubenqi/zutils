// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "db/database_dll.h"

#include "db/mysql/mysql_database.h"

db::BaseDatabase* CreateDatabaseObject() {
  return new db::MySQLDatabase();
}

void DestroyDatabaseObject(db::BaseDatabase* db) {
  if (db) {
    delete db;
  }
}

