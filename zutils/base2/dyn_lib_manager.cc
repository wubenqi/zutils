// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//
//	参考Ogre的部分代码
//

#include "base2/dyn_lib_manager.h"
#include "base2/dyn_lib.h"

namespace base {

//-----------------------------------------------------------------------
DynLibManager::DynLibManager() {
}

//-----------------------------------------------------------------------
DynLib* DynLibManager::load( const std::string& filename) {
	DynLibList::iterator i = mLibList.find(filename);
	if (i != mLibList.end()) {
		return i->second;
	} else {
        DynLib* pLib = new DynLib(filename);
		pLib->load();        
    	mLibList[filename] = pLib;
        return pLib;
	}
}

//-----------------------------------------------------------------------
void DynLibManager::unload(DynLib* lib) {
	DynLibList::iterator i = mLibList.find(lib->getName());
	if (i != mLibList.end()) {
		mLibList.erase(i);
	}
	lib->unload();
	delete lib;
}

//-----------------------------------------------------------------------
DynLibManager::~DynLibManager() {
    // Unload & delete resources in turn
    for( DynLibList::iterator it = mLibList.begin(); it != mLibList.end(); ++it ) {
        it->second->unload();
        delete it->second;
    }

    // Empty the list
    mLibList.clear();
}

}
