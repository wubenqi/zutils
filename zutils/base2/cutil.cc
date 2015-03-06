/**
 * cutil.cpp 
 * DESCRIPTION:
 *	
 * Copyright (C) 2008, www.soucheng.com
 * By Wu Benqi(wubenqi@youcity.com), 2008-06-21
 */

#include <cstring>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <base2/cutil.h>

using std::string;
using std::tolower;

#ifndef HAVE_STRNSTR
char*
strnstr(const char* big, const char* small, unsigned n)
{
	for (unsigned i=0; i < n && big[i] != '\0'; i++)
	{
		unsigned j;
		for (j=0; small[j] != 0; j++)
		{
			if (big[i+j] != small[j])
				break;
		}
		if (small[j]==0)  // find!
		{
			return (char*)(big+i);
		}
	}
	return 0;
}
#endif //HAVE_STRNSTR

#ifndef HAVE_STRCASESTR
char*
strcasestr(const char* big, const char* small)
{
	for (unsigned i=0; big[i] != '\0'; i++)
	{
		unsigned j;
		for (j=0; small[j] != 0; j++)
		{
			if (tolower(big[i+j]) != tolower(small[j]))
				break;
		}
		if (small[j]==0)  // find!
		{
			return (char*)(big+i);
		}
	}
	return 0;
}
#endif //HAVE_STRCASESTR

#ifndef _GNU_SOURCE
char* 
strndup(const char* s, size_t n)
{
	char* target = (char*)malloc(n+1);
	if (target==0)
		return 0;
	strncpy(target, s, n);
	target[n] = '\0';
	return target;
}

int 
getline(char** lineptr, size_t *n, FILE *stream)
{
	unsigned len=0;
	if (*lineptr == 0)
	{
		*n = 256;
		if ((*lineptr = (char*)malloc(*n)))
			return -1;
	}
	if (fgets(*lineptr, *n, stream) == 0)
		return -1;
	len = strlen(*lineptr);
	while (len == *n-1 && *lineptr[*n-2] != '\n')
	{
		*n *= 2;
		char* tmp = (char*)realloc(*lineptr, *n);
		if (tmp==0)
			return -1;
		*lineptr = tmp;
		if (fgets(*lineptr+len, *n-len, stream) == 0)
			return -1;
		len = strlen(*lineptr);
	}
	return 0;
}
#endif // _GNU_SOURCE

string 
tolower(string const &s)
{
	string res(s);
	for (unsigned i = 0; i<res.size(); i++)
	{
		res[i] = tolower(res[i]);
	}
	return res;
}
