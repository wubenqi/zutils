/**
 * cutil.h 
 * DESCRIPTION:
 *	
 * Copyright (C) 2008, www.soucheng.com
 * By Wu Benqi(wubenqi@youcity.com), 2008-06-21
 */

#ifndef __CUTIL_H_
#define __CUTIL_H_

#ifndef WIN32 
#define NO_STRNSTR
#define HAVE_STRCASESTR
#endif

#include <cstdio>
#include <string>
using std::string;

#ifndef HAVE_STRNSTR
#ifndef WIN32
#define HAVE_STRNSTR
#endif
char* strnstr(const char* big, const char* small, unsigned n);
#endif // HAVE_STRNSTR

#ifndef HAVE_STRCASESTR
#ifndef WIN32
#define HAVE_STRCASESTR
#endif
char* strcasestr(const char* big, const char* small);
#endif // HAVE_STRCASESTR

#ifndef _GNU_SOURCE
char* strndup(const char* s, size_t n);
int getline(char **lineptr, size_t *n, FILE *stream);
#endif //_GNU_SOURCE

string tolower(string const &s);

#endif // __CUTIL_H_
