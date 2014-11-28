// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef BASE2_FACTORY_OBJECT_H_
#define BASE2_FACTORY_OBJECT_H_
#pragma once

#include <string>

template< typename T > class FactoryObject {
public:
    virtual ~FactoryObject() {}

	virtual const std::string& GetType() const = 0;

	virtual T* CreateInstance( const std::string& name ) = 0;    
	virtual void DestroyInstance( T* ) = 0;    

};

#endif
