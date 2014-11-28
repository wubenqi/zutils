// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//


#ifndef NET_BASE_DATA_IO_BUFFER_H_
#define NET_BASE_DATA_IO_BUFFER_H_
#pragma once

#include <string>

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"

namespace net {

class DataIOBuffer : 
	public base::RefCountedThreadSafe<DataIOBuffer> {
public:
	DataIOBuffer();
	explicit DataIOBuffer(int size);
	explicit DataIOBuffer(const char* data, int data_size);

	char* data() { return data_.get(); }
	int size() { return size_; }
	void release();

	char* read_data();
	int read_data_size();
	int read_pos();
	bool skip_read_pos(int size);
	bool seek_read_pos(int size);

	char* write_data();
	int write_data_size();
	int write_pos();

	void remove_read_data();
	void remove_write_data();

	bool append(const char* data, int data_size);

	void reset();

	DataIOBuffer* Clone();

private:
	friend class base::RefCountedThreadSafe<DataIOBuffer>;

	virtual ~DataIOBuffer();

	scoped_ptr_malloc<char> data_;

	int size_;		// 数据块大小
	int read_pos_;		// 读位置
	int write_pos_;		// 写位置
};


}  // namespace net

#endif  // NET_BASE_IO_BUFFER_H_
