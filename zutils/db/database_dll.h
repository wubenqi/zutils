// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef DB_DATABASE_DLL_H_
#define DB_DATABASE_DLL_H_

namespace db {
class BaseDatabase;
}

extern "C" db::BaseDatabase* CreateDatabaseObject();
extern "C" void DestroyDatabaseObject(db::BaseDatabase* db);


typedef db::BaseDatabase* (*FN_CreateDatabaseObject)();
typedef void (*FN_DestroyDatabaseObject)(db::BaseDatabase* db);


#endif /* defined(DB_DATABASE_DLL_H_) */

