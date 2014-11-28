// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_BASE_IO_BUFFER_H_
#define NET_BASE_IO_BUFFER_H_
#pragma once

#include <string>

#include "base/pickle.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"

namespace net {

// This is a simple wrapper around a buffer that provides ref counting for
// easier asynchronous IO handling.
class IOBuffer : public base::RefCountedThreadSafe<IOBuffer> {
 public:
  IOBuffer();
  explicit IOBuffer(int data_len);
  IOBuffer(char* data, int data_len, bool is_new=false);

  char* data() { return data_; }
  int   data_len() { return data_len_; }
  bool  is_new() { return is_new_; }

 protected:
  friend class base::RefCountedThreadSafe<IOBuffer>;

  // Only allow derived classes to specify data_.
  // In all other cases, we own data_, and must delete it at destruction time.
  explicit IOBuffer(char* data);

  virtual ~IOBuffer();

  char* data_;
  int   data_len_;
  bool  is_new_;
};

typedef scoped_refptr<IOBuffer> IOBufferPtr;

}  // namespace net

#endif  // NET_BASE_IO_BUFFER_H_
