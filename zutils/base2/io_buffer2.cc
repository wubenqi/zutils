// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "base2/io_buffer2.h"

#include "base/logging.h"
#include "base/stl_util.h"
#include "base/string_number_conversions.h"

//namespace net {

IOBuffer2::IOBuffer2()
: size_(kDefaultIOBufferSize)
, read_pos_(0)
, write_pos_(0)
, is_new_(true) {

  data_ = static_cast<char*>(malloc(kDefaultIOBufferSize));
  CHECK(data_);
}

IOBuffer2::IOBuffer2(int size)
	: size_(size)
	, read_pos_(0)
	, write_pos_(0)
  , is_new_(true) {

	DCHECK(size > 0);
	data_ = static_cast<char*>(malloc(size));
	CHECK(data_);
}

IOBuffer2::IOBuffer2(const void* data, int size)
	: size_(size)
	, read_pos_(0)
	, write_pos_(0)
  , is_new_(false) {

	//DCHECK(data !=NULL && size > 0);
  data_ = static_cast<char*>(const_cast<void*>((data)));
}

IOBuffer2::~IOBuffer2() {
	if (is_new_) {
		free(data_);
	}
}

//void IOBuffer2::remove_read_data() {
//	if (read_pos_>0 && read_remaining()>0) {
//		memmove(data_, read_ptr(), read_remaining());
//		write_pos_ = read_remaining();
//		read_pos_ = 0;
//	} else {
//		read_pos_ = write_pos_ = 0;
//	}
//}
//
//void IOBuffer2::remove_write_data() {
//	if (write_pos_ == 0) {
//		//Release();
//	} else {
//		data_ = static_cast<char*>(realloc(data_, write_pos_));
//		CHECK(data_);
//		size_ = write_pos_;
//	}
//}
//
//bool IOBuffer2::append(const char* data, int data_size) {
//	DCHECK(data !=NULL && data_size > 0);
//	if (write_remaining()<data_size) {
//		size_ += data_size-write_remaining();
//		data_ = static_cast<char*>(realloc(data_, size_));
//		CHECK(data_);
//		//data_.reset(static_cast<char*>(realloc(data_.release(), size_)));
//		memcpy(write_ptr(), data, data_size);
//		write_pos_ = size_;
//	} else {
//		memcpy(write_ptr(), data, data_size);
//		write_pos_+=data_size;
//	}
//	return true;
//}

//void IOBuffer2::release2() {
//	read_pos_ = 0;
//	write_pos_ = 0;
//	size_ = 0;
//	data_ = static_cast<char*>(realloc(data_, inited_size_));
//	CHECK(data_);
//}

IOBuffer2* IOBuffer2::Clone() const {
	IOBuffer2* io_buffer = new IOBuffer2(size_);
  CHECK(io_buffer);
  memcpy(io_buffer->data(), data_, size_);
  return io_buffer;
}

std::string IOBuffer2::DumpHexEncode() const {
  return base::HexEncode(data_, size_);
}

void IOBuffer2::Resize(int size) {
	if (size_<size) {
    if (is_new_) {
      data_ = static_cast<char*>(realloc(data_, size));
      CHECK(data_);
      size_ = size;
    } else {
      char* data = static_cast<char*>(malloc(size));
      CHECK(data);
      memcpy(data, data_, size_);
      size_ = size;
      data_ = data;
      is_new_ = true;
    }
	}
}

bool IOBuffer2::ReadUint8(uint8* val) {
  if (!val || sizeof(*val) > readable_size()) {
    LOG(ERROR) << "ReadUint8 error, val is null or not enough readable_size";
    return false;
  }
  ReadRaw(val, sizeof(*val), false);
  return true;
}

bool IOBuffer2::ReadUint16(uint16* val) {
  if (!val || sizeof(*val) > readable_size()) {
    LOG(ERROR) << "ReadUint16 error, val is null or not enough readable_size";
    return false;
  }
  ReadRaw(val, sizeof(*val), false);
  return true;
}

bool IOBuffer2::ReadUint32(uint32* val) {
  if (!val || sizeof(*val) > readable_size()) {
    LOG(ERROR) << "ReadUint32 error, val is null or not enough readable_size";
    return false;
  }
  ReadRaw(val, sizeof(*val), false);
  return true;
}

bool IOBuffer2::ReadUint64(uint64* val) {
  if (!val || sizeof(*val) > readable_size()) {
    LOG(ERROR) << "ReadUint64 error, val is null or not enough readable_size";
    return false;
  }
  ReadRaw(val, sizeof(*val), false);
  return true;
}

bool IOBuffer2::ReadString(std::string* val) {
  if (!val || sizeof(uint32) > readable_size()) {
    LOG(ERROR) << "ReadString error, val is null or not enough readable_size";
    return false;
  }

  uint32 string_size = 0;
  ReadPeekUint32(&string_size);
  if (string_size+sizeof(uint32) > readable_size()) {
    LOG(ERROR) << "ReadString error, not enough readable_size";
    return false;
  }
  val->resize(string_size);
  ReadOffsetRaw(string_as_array(val), sizeof(string_size), string_size);
  skip_read_pos(string_size+sizeof(string_size));
  return true;
}

bool IOBuffer2::ReadPeekUint8(uint8* val) {
  if (!val || sizeof(*val) > readable_size()) {
    LOG(ERROR) << "ReadPeekUint8 error, val is null or not enough readable_size";
    return false;
  }
  ReadRaw(val, sizeof(*val), true);
  return true;
}

bool IOBuffer2::ReadPeekUint16(uint16* val) {
  if (!val || sizeof(*val) > readable_size()) {
    LOG(ERROR) << "ReadPeekUint16 error, val is null or not enough readable_size";
    return false;
  }
  ReadRaw(val, sizeof(*val), true);
  return true;
}

bool IOBuffer2::ReadPeekUint32(uint32* val) {
  if (!val || sizeof(*val) > readable_size()) {
    LOG(ERROR) << "ReadPeekUint32 error, val is null or not enough readable_size";
    return false;
  }
  ReadRaw(val, sizeof(*val), true);
  return true;
}

bool IOBuffer2::ReadPeekUint64(uint64* val) {
  if (!val || sizeof(*val) > readable_size()) {
    LOG(ERROR) << "ReadPeekUint64 error, val is null or not enough readable_size";
    return false;
  }
  ReadRaw(val, sizeof(*val), true);
  return true;
}

bool IOBuffer2::ReadPeekString(std::string* val) {
  if (!val || sizeof(uint32) > readable_size()) {
    LOG(ERROR) << "ReadPeekString error, val is null or not enough readable_size";
    return false;
  }

  uint32 string_size = 0;
  ReadPeekUint32(&string_size);
  if (string_size+sizeof(uint32) > readable_size()) {
    LOG(ERROR) << "ReadPeekString error, not enough readable_size";
    return false;
  }
  val->resize(string_size);
  ReadOffsetRaw(string_as_array(val), sizeof(string_size), string_size);
  return true;
}


bool IOBuffer2::ReadOffsetUint8(uint8* val, int offset) {
  if (!val || offset+sizeof(*val) > readable_size()) {
    LOG(ERROR) << "ReadOffsetUint8 error, val is null or not enough readable_size";
    return false;
  }
  ReadOffsetRaw(val, sizeof(*val), offset);
  return true;
}

// Ð¡¶Ë
bool IOBuffer2::ReadOffsetUint16(uint16* val, int offset) {
  if (!val || offset+sizeof(*val) > readable_size()) {
    LOG(ERROR) << "ReadOffsetUint16 error, val is null or not enough readable_size";
    return false;
  }
  ReadOffsetRaw(val, sizeof(*val), offset);
  return true;
}

bool IOBuffer2::ReadOffsetUint32(uint32* val, int offset) {
  if (!val || offset+sizeof(*val) > readable_size()) {
    LOG(ERROR) << "ReadOffsetUint32 error, val is null or not enough readable_size";
    return false;
  }
  ReadOffsetRaw(val, sizeof(*val), offset);
  return true;
}

bool IOBuffer2::ReadOffsetUint64(uint64* val, int offset) {
  if (!val || offset+sizeof(*val) > readable_size()) {
    LOG(ERROR) << "ReadOffsetUint64 error, val is null or not enough readable_size";
    return false;
  }
  ReadOffsetRaw(val, sizeof(*val), offset);
  return true;
}

bool IOBuffer2::ReadOffsetString(std::string* val, int offset) {
  if (!val || sizeof(uint32)+offset > readable_size()) {
    LOG(ERROR) << "ReadOffsetString error, val is null or not enough readable_size";
    return false;
  }

  uint32 string_size = 0;
  ReadPeekUint32(&string_size);
  if (string_size+sizeof(uint32)+offset > readable_size()) {
    LOG(ERROR) << "ReadPeekString error, not enough readable_size";
    return false;
  }
  val->resize(string_size);
  ReadOffsetRaw(string_as_array(val), sizeof(string_size)+offset, string_size);
  return true;
}


void IOBuffer2::WriteUint8(uint8 val) {
	WriteRaw(&val, sizeof(val));
}

// Ð¡¶Ë
void IOBuffer2::WriteUint16(uint16 val) {
	WriteRaw(&val, sizeof(val));
}

void IOBuffer2::WriteUint32(uint32 val) {
	WriteRaw(&val, sizeof(val));
}

void IOBuffer2::WriteUint64(uint64 val) {
  WriteRaw(&val, sizeof(val));
}

void IOBuffer2::WriteString(const std::string& val) {
  uint32 string_size = val.length()+1;
  WriteUint32(string_size);
  WriteRaw(val.c_str(), string_size);
}

void IOBuffer2::WriteString(const char* val) {
  uint32 string_size = strlen(val)+1;
  WriteUint32(string_size);
  WriteRaw(val, string_size);
}

void IOBuffer2::WriteOffsetUint8(uint8 val, int offset) {
	return WriteOffsetRaw(&val, sizeof(val), offset);
}

// Ð¡¶Ë
void IOBuffer2::WriteOffsetUint16(uint16 val, int offset) {
	return WriteOffsetRaw(&val, sizeof(val), offset);
}

void IOBuffer2::WriteOffsetUint32(uint32 val, int offset) {
	return WriteOffsetRaw(&val, sizeof(val), offset);
}

void IOBuffer2::WriteOffsetUint64(uint64 val, int offset) {
  return WriteOffsetRaw(&val, sizeof(val), offset);
}

void IOBuffer2::WriteOffsetString(const std::string& val, int offset) {
  uint32 string_size = val.length()+1;
  WriteOffsetUint32(string_size, offset);
  WriteOffsetRaw(val.c_str(), string_size, offset+sizeof(uint32));
}

void IOBuffer2::WriteOffsetString(const char* val, int offset) {
  uint32 string_size = strlen(val)+1;
  WriteOffsetUint32(string_size, offset);
  WriteOffsetRaw(val, string_size, offset+sizeof(uint32));
}

//>>====================================================================================
void IOBuffer2::ReadRaw(void* buf, int buf_len, bool is_peek) {
  memcpy(buf, read_ptr(), buf_len);
  if (!is_peek) {
    read_pos_ += buf_len;
  }
}

void IOBuffer2::ReadOffsetRaw(void* buf, int offset, int buf_len) {
  memcpy(buf, read_ptr()+offset, buf_len);
}

// Ð´
void IOBuffer2::WriteRaw(const void* buf, int buf_len) {
  if (size_-write_pos_<buf_len) {
    int new_size = size_-write_pos_ + buf_len;
    if (is_new_) {
      data_ = static_cast<char*>(realloc(data_, new_size));
      CHECK(data_);
    } else {
      char* new_data = static_cast<char*>(malloc(new_size));
      CHECK(new_data);
      memcpy(new_data, data_, size_);
      data_ = new_data;
      is_new_ = true;
    }
    size_ = new_size;
  }

  memcpy(write_ptr(), buf, buf_len);
  write_pos_ += buf_len;
}

void IOBuffer2::WriteOffsetRaw(const void* buf, int buf_len, int offset) {
  if (size_-write_pos_<buf_len+offset) {
    int new_size = size_-write_pos_ + buf_len+offset;
    if (is_new_) {
      data_ = static_cast<char*>(realloc(data_, new_size));
      CHECK(data_);
    } else {
      char* new_data = static_cast<char*>(malloc(new_size));
      CHECK(new_data);
      memcpy(new_data, data_, size_);
      data_ = new_data;
      is_new_ = false;
    }
    size_ = new_size;
  }

  memcpy(write_ptr()+offset, buf, buf_len);
}
