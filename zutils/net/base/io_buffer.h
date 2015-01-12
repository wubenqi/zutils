// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// 原始出处：
// https://github.com/chenshuo/muduo/blob/master/muduo/net/Buffer.h

#ifndef NET_BASE_IO_BUFFER_H_
#define NET_BASE_IO_BUFFER_H_

#include <algorithm>

#include "base/sys_byteorder.h"
#include "base/strings/string_piece.h"
#include "base/logging.h"

namespace net {

/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode

// This is a simple wrapper around a buffer that provides ref counting for
// easier asynchronous IO handling.

class IOBuffer {
 public:
   static const size_t kCheapPrepend = 8;
   static const size_t kInitialSize = 1024;

   explicit IOBuffer(size_t initial_size = kInitialSize)
     : buffer_(kCheapPrepend + initial_size),
       reader_index_(kCheapPrepend),
       writer_index_(kCheapPrepend) {
     DCHECK(ReadableBytes() == 0);
     DCHECK(WritableBytes() == initial_size);
     DCHECK(PrependableBytes() == kCheapPrepend);
   }

   // implicit copy-ctor, move-ctor, dtor and assignment are fine
   // NOTE: implicit move-ctor is added in g++ 4.6

   void Swap(IOBuffer& rhs) {
     buffer_.swap(rhs.buffer_);
     std::swap(reader_index_, rhs.reader_index_);
     std::swap(writer_index_, rhs.writer_index_);
   }

   size_t ReadableBytes() const {
     return writer_index_ - reader_index_;
   }

   size_t WritableBytes() const {
     return buffer_.size() - writer_index_;
   }

   size_t PrependableBytes() const {
     return reader_index_;
   }

   const char* Peek() const {
     return Begin() + reader_index_;
   }

   const char* FindCRLF() const {
     // FIXME: replace with memmem()?
     const char* crlf = std::search(Peek(), BeginWrite(), kCRLF, kCRLF+2);
     return crlf == BeginWrite() ? NULL : crlf;
   }

   const char* FindCRLF(const char* start) const {
     DCHECK(Peek() <= start);
     DCHECK(start <= BeginWrite());
     // FIXME: replace with memmem()?
     const char* crlf = std::search(start, BeginWrite(), kCRLF, kCRLF+2);
     return crlf == BeginWrite() ? NULL : crlf;
   }

   const char* FindEOL() const {
     const void* eol = memchr(Peek(), '\n', ReadableBytes());
     return static_cast<const char*>(eol);
   }

   const char* FindEOL(const char* start) const {
     DCHECK(Peek() <= start);
     DCHECK(start <= BeginWrite());
     const void* eol = memchr(start, '\n', BeginWrite() - start);
     return static_cast<const char*>(eol);
   }

   // retrieve returns void, to prevent
   // string str(retrieve(readableBytes()), readableBytes());
   // the evaluation of two functions are unspecified
   void Retrieve(size_t len) {
     DCHECK(len <= ReadableBytes());
     if (len < ReadableBytes()) {
       reader_index_ += len;
     } else {
       RetrieveAll();
     }
   }

   void RetrieveUntil(const char* end) {
     DCHECK(Peek() <= end);
     DCHECK(end <= BeginWrite());
     Retrieve(end - Peek());
   }

   void RetrieveInt64() {
     Retrieve(sizeof(int64));
   }

   void RetrieveInt32() {
     Retrieve(sizeof(int32));
   }

   void RetrieveInt16() {
     Retrieve(sizeof(int16));
   }

   void RetrieveInt8() {
     Retrieve(sizeof(int8));
   }

   void RetrieveAll() {
     reader_index_ = kCheapPrepend;
     writer_index_ = kCheapPrepend;
   }

   std::string RetrieveAllAsString() {
     return RetrieveAsString(ReadableBytes());;
   }

   std::string RetrieveAsString(size_t len) {
     DCHECK(len <= ReadableBytes());
     std::string result(Peek(), len);
     Retrieve(len);
     return result;
   }

   base::StringPiece ToStringPiece() const {
     return base::StringPiece(Peek(), static_cast<int>(ReadableBytes()));
   }

   void Append(const base::StringPiece& str) {
     Append(str.data(), str.size());
   }

   void Append(const char* /*restrict*/ data, size_t len) {
     EnsureWritableBytes(len);
     std::copy(data, data+len, BeginWrite());
     HasWritten(len);
   }

   void Append(const void* /*restrict*/ data, size_t len) {
     Append(static_cast<const char*>(data), len);
   }

   void EnsureWritableBytes(size_t len) {
     if (WritableBytes() < len) {
       MakeSpace(len);
     }
     DCHECK(WritableBytes() >= len);
   }

   char* BeginWrite() {
     return Begin() + writer_index_;
   }

   const char* BeginWrite() const {
     return Begin() + writer_index_;
   }

   void HasWritten(size_t len) {
     DCHECK(len <= WritableBytes());
     writer_index_ += len;
   }

   void Unwrite(size_t len) {
     DCHECK(len <= ReadableBytes());
     writer_index_ -= len;
   }

   ///
   /// Append int64 using network endian
   ///
   void AppendInt64(int64 x) {
     AppendUint64(x);
   }

   void AppendUint64(int64 x) {
     uint64 be64 = base::HostToNet64(x);
     Append(&be64, sizeof(be64));
   }

   ///
   /// Append int32 using network endian
   ///
   void AppendInt32(int32 x) {
     AppendUint32(x);
   }

   void AppendUint32(int32 x) {
     uint32 be32 = base::HostToNet32(x);
     Append(&be32, sizeof(be32));
   }

   void AppendInt16(int16 x) {
     AppendUint16(x);
   }

   void AppendUint16(int16 x) {
     uint16 be16 = base::HostToNet16(x);
     Append(&be16, sizeof(be16));
   }

   void AppendInt8(int8 x) {
     Append(&x, sizeof(x));
   }

   void AppendUint8(int8 x) {
     Append(&x, sizeof(x));
   }

   ///
   /// Read int64 from network endian
   ///
   /// Require: buf->readableBytes() >= sizeof(int32)
   int64 ReadInt64() {
     int64 result = PeekInt64();
     RetrieveInt64();
     return result;
   }

   uint64 ReadUint64() {
     uint64 result = PeekUint64();
     RetrieveInt64();
     return result;
   }

   ///
   /// Read int32 from network endian
   ///
   /// Require: buf->readableBytes() >= sizeof(int32)
   int32 ReadInt32() {
     int32 result = PeekInt32();
     RetrieveInt32();
     return result;
   }

   uint32 ReadUint32() {
     uint32 result = PeekUint32();
     RetrieveInt32();
     return result;
   }

   int16 ReadInt16() {
     int16 result = PeekInt16();
     RetrieveInt16();
     return result;
   }

   uint16 ReadUint16() {
     uint16 result = PeekUint16();
     RetrieveInt16();
     return result;
   }

   int8 ReadInt8() {
     int8 result = PeekInt8();
     RetrieveInt8();
     return result;
   }

   uint8 ReadUint8() {
     uint8 result = PeekUint8();
     RetrieveInt8();
     return result;
   }

   ///
   /// Peek int64 from network endian
   ///
   /// Require: buf->readableBytes() >= sizeof(int64)
   int64 PeekInt64() const {
     return static_cast<int64>(PeekUint64());
   }

   uint64 PeekUint64() const {
     DCHECK(ReadableBytes() >= sizeof(int64));
     uint64 be64 = 0;
     ::memcpy(&be64, Peek(), sizeof(be64));
     return base::NetToHost64(be64);
   }

   ///
   /// Peek int32 from network endian
   ///
   /// Require: buf->readableBytes() >= sizeof(int32)
   int32 PeekInt32() const {
     return static_cast<int32>(PeekUint32());
   }

   uint32 PeekUint32() const {
     DCHECK(ReadableBytes() >= sizeof(int32));
     uint32 be32 = 0;
     ::memcpy(&be32, Peek(), sizeof(be32));
     return base::NetToHost32(be32);
   }

   int16 PeekInt16() const {
     return static_cast<int16>(PeekUint16());
   }

   uint16 PeekUint16() const {
     DCHECK(ReadableBytes() >= sizeof(int16));
     uint16 be16 = 0;
     ::memcpy(&be16, Peek(), sizeof(be16));
     return base::NetToHost16(be16);
   }

   int8 PeekInt8() const {
     DCHECK(ReadableBytes() >= sizeof(int8));
     int8 x = *Peek();
     return x;
   }

   uint8 PeekUint8() const {
     DCHECK(ReadableBytes() >= sizeof(int8));
     uint8 x = *Peek();
     return x;
   }

   ///
   /// Prepend int64 using network endian
   ///
   void PrependInt64(int64 x) {
     PrependUint64(x);
   }

   void PrependUint64(uint64 x) {
     uint64 be64 = base::HostToNet64(x);
     Prepend(&be64, sizeof be64);
   }

   ///
   /// Prepend int32 using network endian
   ///
   void PrependInt32(int32 x) {
     PrependUint32(x);
   }

   void PrependUint32(uint32 x) {
     uint32 be32 = base::HostToNet32(x);
     Prepend(&be32, sizeof be32);
   }

   void PrependInt16(int16 x) {
     PrependUint16(x);
   }

   void PrependUint16(uint16 x) {
     int16 be16 = base::HostToNet16(x);
     Prepend(&be16, sizeof be16);
   }

   void PrependInt8(int8 x) {
     Prepend(&x, sizeof x);
   }

   void PrependUint8(uint8 x) {
     Prepend(&x, sizeof x);
   }

   void Prepend(const void* /*restrict*/ data, size_t len) {
     DCHECK(len <= PrependableBytes());
     reader_index_ -= len;
     const char* d = static_cast<const char*>(data);
     std::copy(d, d+len, Begin()+reader_index_);
   }

   void Shrink(size_t reserve) {
     // FIXME: use vector::shrink_to_fit() in C++ 11 if possible.
     IOBuffer other;
     other.EnsureWritableBytes(ReadableBytes()+reserve);
     other.Append(ToStringPiece());
     Swap(other);
   }

   size_t InternalCapacity() const {
     return buffer_.capacity();
   }

   /// Read data directly into buffer.
   ///
   /// It may implement with readv(2)
   /// @return result of read(2), @c errno is saved
   int ReadFd(int fd, int* saved_errno);

private:

  char* Begin() {
    return &*buffer_.begin();
  }

  const char* Begin() const {
    return &*buffer_.begin();
  }

  void MakeSpace(size_t len) {
    if (WritableBytes() + PrependableBytes() < len + kCheapPrepend) {
      // FIXME: move readable data
      buffer_.resize(writer_index_+len);
    } else {
      // move readable data to the front, make space inside buffer
      DCHECK(kCheapPrepend < reader_index_);
      size_t readable = ReadableBytes();
      std::copy(Begin()+reader_index_,
        Begin()+writer_index_,
        Begin()+kCheapPrepend);
      reader_index_ = kCheapPrepend;
      writer_index_ = reader_index_ + readable;
      DCHECK(readable == ReadableBytes());
    }
  }

private:
  std::vector<char> buffer_;
  size_t reader_index_;
  size_t writer_index_;

  static const char kCRLF[];
};

}  // namespace net

#endif  // NET_BASE_IO_BUFFER_H_
