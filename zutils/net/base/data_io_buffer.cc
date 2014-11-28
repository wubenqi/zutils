// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/base/data_io_buffer.h"

#include "base/logging.h"

namespace net {

DataIOBuffer::DataIOBuffer() {
	data_.reset(static_cast<char*>(malloc(4096)));
	size_ = 4096;
	read_pos_ = 0;
	write_pos_ = 0;
}

DataIOBuffer::DataIOBuffer(int size) {
  DCHECK(size > 0);
  data_.reset(static_cast<char*>(malloc(size)));
  size_ = size;
  read_pos_ = 0;
  write_pos_ = 0;
}

DataIOBuffer::DataIOBuffer(const char* data, int data_size) {
	DCHECK(data !=NULL && data_size > 0);
	data_.reset(static_cast<char*>(malloc(data_size)));
	memcpy(data_.get(), data, data_size);
	size_ = data_size;
	read_pos_ = 0;
	write_pos_ = data_size;
}

DataIOBuffer::~DataIOBuffer() {
	//data_ = NULL;
}

char* DataIOBuffer::read_data() {
	return data_.get()+read_pos_;
}

int DataIOBuffer::read_pos() {
	return read_pos_;
}

int DataIOBuffer::read_data_size() { 
	return write_pos_-read_pos_;
}

bool DataIOBuffer::skip_read_pos(int size) {
	if (size<0) {
		if (read_pos_+size<0) {
			read_pos_ = 0;
		}
	}
	
	if (size>read_data_size()) {
		size = read_data_size();
	}
	read_pos_+=size;
	return true;
}

bool DataIOBuffer::seek_read_pos(int size) {
	if (size>size_) {
		return false;
	}
	read_pos_ = size;
	//write_pos_ = size;
	
	return true;
}

char* DataIOBuffer::write_data() {
	return data_.get()+write_pos_;
}

int DataIOBuffer::write_data_size() {
	return size_-write_pos_;
}

int DataIOBuffer::write_pos() {
	return write_pos_;
}

void DataIOBuffer::remove_read_data() {
	if (read_pos_==0) {
		// ²»±ä
	} else if (read_pos_>0 && read_data_size()>0) {
		memmove(data_.get(), read_data(), read_data_size());
		write_pos_ = read_data_size();
		read_pos_ = 0;
	} else {
		read_pos_ = write_pos_ = 0;
	}
}

void DataIOBuffer::remove_write_data() {
	if (write_pos_ == 0) {
		Release();
	} else {
		data_.reset(static_cast<char*>(realloc(data_.release(), write_pos_)));
		size_ = write_pos_;
	}
}

void DataIOBuffer::reset() {
	read_pos_ = 0;
	write_pos_ = 0;
}

bool DataIOBuffer::append(const char* data, int data_size) {
	DCHECK(data !=NULL && data_size > 0);
	if (write_data_size()<data_size) {
		size_ += data_size-write_data_size();
		data_.reset(static_cast<char*>(realloc(data_.release(), size_)));
		memcpy(write_data(), data, data_size);
		write_pos_ = size_;
	} else {
		memcpy(write_data(), data, data_size);
		write_pos_+=data_size;
	}
	return true;
}

void DataIOBuffer::release() {
	read_pos_ = 0;
	write_pos_ = 0;
	size_ = 0;
	data_.reset(static_cast<char*>(realloc(data_.release(), 4096)));
}

DataIOBuffer* DataIOBuffer::Clone() {
	return new DataIOBuffer(data_.get(), write_pos_);
}

}  // namespace net
