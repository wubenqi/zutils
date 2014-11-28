// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/base/io_buffer.h"

#include "base/logging.h"

namespace net {

IOBuffer::IOBuffer()
  : data_(NULL)
  , data_len_(0)
  , is_new_(false) {
}

IOBuffer::IOBuffer(int data_len) {
  DCHECK(data_len > 0);
  data_ = new char[data_len];
  data_len_ = data_len;
  is_new_ = true;
}

IOBuffer::IOBuffer(char* data, int data_len, bool is_new) {
  DCHECK(data_len > 0);
  DCHECK(data);
  data_len_ = data_len;
  is_new_ = is_new;
  if (is_new) {
    data_ = new char[data_len];
    memcpy(data_, data, data_len);
  } else {
    data_ = data;
  }
}

IOBuffer::~IOBuffer() {
  if (is_new_) {
    delete[] data_;
  }
}

}  // namespace net
