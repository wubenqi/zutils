// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "base2/zlib_compress.h"

#include <zlib.h>
#include "base/logging.h"

namespace base {

static inline uint32 ReadUint32(const char* data) {
	return (( data[0] & 0xFF) << 24) | ((data[1] & 0xFF) << 16) | ((data[2] & 0xFF) <<  8) | (data[3] & 0xFF);
}

static inline void WriteUint32(char* out_data, uint32 in_num) {
	out_data[0] = (in_num >> 24) & 0xFF;
	out_data[1] = (in_num >> 16) & 0xFF;
	out_data[2] = (in_num >> 8) & 0xFF;
	out_data[3] = in_num & 0xFF;
}

void ZlibCompress::Compress(const char* in_data, uint32 in_len, char** out_data, uint32* out_len) {
	uint32 headder_flag = in_len;

	if ((Compress1(in_data, in_len, out_data, out_len) != Z_OK)) {
		memcpy(*out_data+sizeof(uint32), in_data, in_len);
		*out_len = in_len+sizeof(uint32);
		headder_flag = 0xFFFFFFFF;
	}

	WriteUint32(*out_data, headder_flag);
}

bool ZlibCompress::UnCompress(const char* in_data, uint32 in_len, char** out_data, uint32* out_len) {
	bool result = false;

	if (in_len>=sizeof(uint32)) {
		*out_data = UnCompress1(in_data, in_len, out_len);
		result = (*out_data!=NULL);
	}

	return result;
}

char* ZlibCompress::UnCompress1(const char* in_data, uint32 in_len, uint32* out_len) {
	char* uc = NULL;
	uint32 u_len = ReadUint32(in_data);
	if (u_len==0xFFFFFFFF) {
		u_len = in_len-sizeof(uint32);
		uc = new char[u_len];
		CHECK(uc) << "Alloc memory error!";
		memcpy(uc, in_data+sizeof(uint32), u_len);
	} else {
		uc = new char[u_len];
		CHECK(uc) << "Alloc memory error!";
		if(uncompress((Bytef*)uc, (uLongf *)(&u_len), (const Bytef *)(in_data+sizeof(uint32)), *out_len)!=Z_OK) {
			delete [] uc;
			uc = NULL;
			LOG(ERROR) << "Uncompress data error! in_data fomat valid?";
		}
	}
	return uc;
}

int ZlibCompress::Compress1(const char* in_data, uint32 in_len, char** out_data, uint32* out_len) {
	uint32 o_len = in_len + (in_len/1000) + 12;
	char* o_data = new char[o_len+sizeof(uint32)];
	CHECK(o_data) << "Alloc memory error!";

	o_data += sizeof(uint32);

	int result = compress2((Bytef*)o_data, (uLongf *)(&o_len), (const Bytef *)in_data, in_len, 5);
	
	*out_len = o_len + sizeof(uint32);
	*out_data = o_data-sizeof(uint32);

	return result;
}

}

