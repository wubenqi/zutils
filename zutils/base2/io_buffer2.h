// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//
// 简单随机读写IOBuffer
// 只维护读写指针
//

#ifndef BASE2_IO_BUFFER2_H_
#define BASE2_IO_BUFFER2_H_
#pragma once


#include "base/basictypes.h"

#include <string>

class IOBuffer2 {
public:
	enum {
		kDefaultIOBufferSize = 4096
	};

  IOBuffer2();
	explicit IOBuffer2(int size);
	IOBuffer2(const void* data, int size);
	virtual ~IOBuffer2();

	//>>====================================================================================
	inline char* data() const {
		return data_;
	}

	inline int size() const {
		return size_;
	}

  //char* GetData() const {
  //  return data+current_pos;
  //}

  //int GetDataLen() const {
  //  return data_len-current_pos;
  //}

	inline char* read_ptr() const {
		return data_+read_pos_;
	}

  inline int readable_size() const {
    return size_-read_pos_;
  }

	//inline int read_remaining() const {
	//	return write_pos_-read_pos_;
	//}

	inline int read_pos() const {
		return read_pos_;
	}

	inline bool skip_read_pos(int size) {
    int _read_pos = size+read_pos_;
    if (_read_pos<0 || _read_pos>=size_) {
      return false;
    }
    read_pos_ = _read_pos;
    return true;
	}

	inline bool seek_read_pos(int size) {
		if (size_<0 || size>=size_) {
			return false;
		}
		read_pos_ = size;
		return true;
	}

	inline char* write_ptr() {
		return data_+write_pos_;
	}

	//inline int write_remaining() {
	//	return size_-write_pos_;
	//}

	inline int write_pos() {
		return write_pos_;
	}

  inline int writable_size() const {
    return size_-write_pos_;
  }

	//void remove_read_data();
	//void remove_write_data();

  inline bool skip_write_pos(int size) {
    int _write_pos = size+write_pos_;
    if (_write_pos<0 || _write_pos>=size_) {
      return false;
    }
    write_pos_ = _write_pos;
    return true;
  }

  inline bool seek_write_pos(int size) {
    if (size_<0 || size>=size_) {
      return false;
    }
    write_pos_ = size;
    return true;
  }

	//bool append(const char* data, int data_size);

	//inline void Recycle() {
	//	read_pos_ = 0;
	//	write_pos_ = 0;
	//}
	
	void Resize(int size);

	//void release2();

	IOBuffer2* Clone() const;
  std::string DumpHexEncode() const;

	//>>====================================================================================
	// 读

  bool ReadUint8(uint8* val);
	bool ReadUint16(uint16* val);
	bool ReadUint32(uint32* val);
  bool ReadUint64(uint64* val);
  bool ReadString(std::string* val);

  bool ReadPeekUint8(uint8* val);
  bool ReadPeekUint16(uint16* val);
  bool ReadPeekUint32(uint32* val);
  bool ReadPeekUint64(uint64* val);
  bool ReadPeekString(std::string* val);

  bool ReadOffsetUint8(uint8* val, int offset);
	bool ReadOffsetUint16(uint16* val, int offset);
	bool ReadOffsetUint32(uint32* val, int offset);
  bool ReadOffsetUint64(uint64* val, int offset);
  bool ReadOffsetString(std::string* val, int offset);

	
  void WriteUint8(uint8 val);
	void WriteUint16(uint16 val);
	void WriteUint32(uint32 val);
  void WriteUint64(uint64 val);
  void WriteString(const std::string& val);
  void WriteString(const char* val);

  void WriteOffsetUint8(uint8 val, int offset);
	void WriteOffsetUint16(uint16 val, int offset);
	void WriteOffsetUint32(uint32 val, int offset);
  void WriteOffsetUint64(uint64 val, int offset);
  void WriteOffsetString(const std::string& val, int offset);
  void WriteOffsetString(const char* val, int offset);

protected:
  // 外部调用需要保证不出错
  void ReadRaw(void* buf, int buf_len, bool is_peek = false);
  void ReadOffsetRaw(void* buf, int offset, int buf_len);

  void WriteRaw(const void* buf, int buf_len);
  void WriteOffsetRaw(const void* buf, int buf_len, int offset);

  //virtual char* Alloc(int size);
  //virtual void  Free();

private:
	char* data_;
	int size_;			// 数据块大小

  int read_pos_;		// 读位置
	int write_pos_;		// 写位置

	//int inited_size_;	
  bool is_new_;
};

#endif  // NET_BASE_IO_BUFFER_H_
