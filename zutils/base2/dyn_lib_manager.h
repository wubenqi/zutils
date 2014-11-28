// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//
//	参考Ogre的部分代码
//

#ifndef BASE2_DYN_LIB_MANAGER_H_
#define BASE2_DYN_LIB_MANAGER_H_

#include <string>
#include <map>
#include "base/memory/singleton.h"

namespace base {

/** Manager for Dynamic-loading Libraries.
 @remarks
 This manager keeps a track of all the open dynamic-loading
 libraries, opens them and returns references to already-open
 libraries.
 */
class DynLib;
class DynLibManager {
public:
  static DynLibManager* GetInstance() {
    return Singleton<DynLibManager>::get();
  }

  /** Loads the passed library.
   @param
   filename The name of the library. The extension can be omitted
   */
  DynLib* load(const std::string& filename);

  /** Unloads the passed library.
   @param
   filename The name of the library. The extension can be omitted
   */
  void unload(DynLib* lib);

protected:
  friend struct DefaultSingletonTraits<DynLibManager>;

  /** Default constructor.
   @note
   <br>Should never be called as the singleton is automatically
   created during the creation of the Root object.
   @see
   Root::Root
   */
  DynLibManager();

  /** Default destructor.
   @see
   Root::~Root
   */
  virtual ~DynLibManager();

  typedef std::map<std::string, DynLib*> DynLibList;
  DynLibList mLibList;
};

}

#endif
