// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// 原始出处：
// https://github.com/chenshuo/muduo/blob/master/muduo/net/Buffer.cc

#include "net/base/io_buffer.h"

#include "base/logging.h"
#include "base/posix/eintr_wrapper.h"

namespace net {

const char IOBuffer::kCRLF[] = "\r\n";

// const size_t IOBuffer::kCheapPrepend;
// const size_t IOBuffer::kInitialSize;

int IOBuffer::ReadFd(int fd, int* saved_errno) {
#if defined(OS_WIN)
  char buf[65536];  // +1 for null termination
  int n = HANDLE_EINTR(recv(fd, buf, 65536, 0));
  if (n > 0) {
    if (n == 65536) {
      // ???
      LOG(ERROR) << "impossible!!!!";
    } else {
      Append(buf, n);
    }
  } else {
    *saved_errno = n;
  }
  return n;
#else
  // saved an ioctl()/FIONREAD call to tell how much to read
  char extrabuf[65536];
  struct iovec vec[2];
  const size_t writable = WritableBytes();
  vec[0].iov_base = Begin()+writer_index_;
  vec[0].iov_len = writable;
  vec[1].iov_base = extrabuf;
  vec[1].iov_len = sizeof extrabuf;
  // when there is enough space in this buffer, don't read into extrabuf.
  // when extrabuf is used, we read 128k-1 bytes at most.
  const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
  const ssize_t n = readv(fd, vec, iovcnt);
  if (n < 0) {
    *saved_errno = errno;
  } else if (implicit_cast<size_t>(n) <= writable) {
    writer_index_ += n;
  } else {
    writer_index_ = buffer_.size();
    Append(extrabuf, n - writable);
  }
  // if (n == writable + sizeof extrabuf)
  // {
  //   goto line_30;
  // }
  return n;
#endif

}


}  // namespace net
