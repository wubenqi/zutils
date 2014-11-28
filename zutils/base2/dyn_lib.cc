// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//
//	参考Ogre的部分代码
//

#ifdef WIN32
//#  define WIN32_LEAN_AND_MEAN
//#  define NOMINMAX // required to stop windows.h messing up std::min
#  include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "base2/exception.h"
#include "base2/dyn_lib.h"


namespace base {

//-----------------------------------------------------------------------
DynLib::DynLib( const std::string& name ) {
    name_ = name;
    m_hInst = NULL;
}

//-----------------------------------------------------------------------
DynLib::~DynLib() {
}

//-----------------------------------------------------------------------
void DynLib::load() {
	std::string name = name_;
#if defined(OS_MACOSX)
  // dlopen() does not add .so to the filename, like windows does for .dll
  if (name.substr(name.length() - 6, 6) != ".dylib")
    name += ".dylib";
#elif defined(OS_LINUX)
  // dlopen() does not add .so to the filename, like windows does for .dll
  if (name.substr(name.length() - 3, 3) != ".so")
    name += ".so";
#endif

    m_hInst = (DYNLIB_HANDLE)DYNLIB_LOAD( name.c_str() );

	if( !m_hInst ) {
		throw InternalErrorException(
				"Could not load dynamic library " + name_ +
				".  System Error: " + dynlibError()
		);
	}
}

//-----------------------------------------------------------------------
void DynLib::unload() {
    if( DYNLIB_UNLOAD( m_hInst ) ) {
		throw InternalErrorException(
				"Could not unload dynamic library " + name_ +
				".  System Error: " + dynlibError()
		);
	}

}

//-----------------------------------------------------------------------
void* DynLib::getSymbol( const std::string& strName ) const throw() {
    return (void*)DYNLIB_GETSYM( m_hInst, strName.c_str() );
}

//-----------------------------------------------------------------------
std::string DynLib::dynlibError( void ) {
#ifdef WIN32
    LPVOID lpMsgBuf; 
    FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS, 
        NULL, 
        GetLastError(), 
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
        (LPTSTR) &lpMsgBuf, 
        0, 
        NULL 
        ); 
    std::string ret = (char*)lpMsgBuf;
    // Free the buffer.
    LocalFree( lpMsgBuf );
    return ret;
#else
    return std::string(dlerror());
#endif
}

}
