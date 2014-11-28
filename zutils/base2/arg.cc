// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

#include "base2/arg.h"

using std::string;
using std::vector;
using std::istringstream;
using std::runtime_error;

using std::strlen;
using std::strncmp;
using std::logic_error;

CArg::CArg(int argc, char** argv) {
	this->argc = argc;
	this->argv = argv;
}

CArg::~CArg() {
}

CArg::ArgVal
CArg::find1(const char* prefix) {
	for (int i=1; i<argc; i++) {
		if (strncmp(argv[i], prefix, strlen(prefix)) == 0) {
			return argv[i]+strlen(prefix);
		}
	}
	return 0;
}

vector<CArg::ArgVal> CArg::find(const char* prefix) {
	vector<ArgVal> res;
	for (int i=1; i<argc; i++) {
		if (strncmp(argv[i], prefix, strlen(prefix)) == 0) {
			res.push_back(argv[i] + strlen(prefix));
		}
			
	}
	return res;
}

CArg::ArgVal::operator const char* () const {
	return charPtr;
}

CArg::ArgVal::operator int () const {
	if (charPtr == 0) 
		throw runtime_error("Convert from null.");
	istringstream iss(charPtr);
	int res;
	if (!(iss>>res))
		throw runtime_error("Convert to int failed.");
	return res;
}

