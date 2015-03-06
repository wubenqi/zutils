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

   void Write(const base::StringPiece& str) {
     Write(str.data(), str.size());
   }

   void Write(const char* /*restrict*/ data, size_t len) {
     EnsureWritableBytes(len);
     std::copy(data, data+len, BeginWrite());
     HasWritten(len);
   }

   void Write(const void* /*restrict*/ data, size_t len) {
     Write(static_cast<const char*>(data), len);
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

   void WriteBigEndianInt64(int64 x) {
     WriteBigEndianUint64(x);
   }

   void WriteBigEndianUint64(int64 x) {
     uint64 be64 = base::HostToNet64(x);
     Write(&be64, sizeof(be64));
   }

   void WriteLittleEndianInt64(int64 x) {
     WriteLittleEndianUint64(x);
   }

   void WriteLittleEndianUint64(int64 x) {
     uint64 be64 = base::ByteSwapToLE64(x);
     Write(&be64, sizeof(be64));
   }

   void WriteBigEndianInt32(int32 x) {
     WriteBigEndianUint32(x);
   }

   void WriteBigEndianUint32(int32 x) {
     uint32 be32 = base::HostToNet32(x);
     Write(&be32, sizeof(be32));
   }

   void WriteLittleEndianInt32(int32 x) {
     WriteLittleEndianUint32(x);
   }

   void WriteLittleEndianUint32(int32 x) {
     uint32 be32 = base::ByteSwapToLE32(x);
     Write(&be32, sizeof(be32));
   }

   void WriteBigEndianInt16(int16 x) {
     WriteBigEndianUint16(x);
   }

   void WriteBigEndianUint16(int16 x) {
     uint16 be16 = base::HostToNet16(x);
     Write(&be16, sizeof(be16));
   }

   void WriteLittleEndianInt16(int16 x) {
     WriteLittleEndianUint16(x);
   }

   void WriteLittleEndianUint16(int16 x) {
     uint16 be16 = base::ByteSwapToLE16(x);
     Write(&be16, sizeof(be16));
   }

   void WriteInt8(int8 x) {
     Write(&x, sizeof(x));
   }

   void WriteUint8(int8 x) {
     Write(&x, sizeof(x));
   }

   int64 ReadBigEndianInt64() {
     int64 result = PeekBigEndianInt64();
     RetrieveInt64();
     return result;
   }

   uint64 ReadBigEndianUint64() {
     uint64 result = PeekBigEndianUint64();
     RetrieveInt64();
     return result;
   }

   int64 ReadLittleEndianInt64() {
     int64 result = PeekLittleEndianInt64();
     RetrieveInt64();
     return result;
   }

   uint64 ReadLittleEndianUint64() {
     uint64 result = PeekLittleEndianUint64();
     RetrieveInt64();
     return result;
   }

   int32 ReadBigEndianInt32() {
     int32 result = PeekBigEndianInt32();
     RetrieveInt32();
     return result;
   }

   uint32 ReadBigEndianUint32() {
     uint32 result = PeekBigEndianUint32();
     RetrieveInt32();
     return result;
   }

   int32 ReadLittleEndianInt32() {
     int32 result = PeekLittleEndianInt32();
     RetrieveInt32();
     return result;
   }

   uint32 ReadLittleEndianUint32() {
     uint32 result = PeekLittleEndianUint32();
     RetrieveInt32();
     return result;
   }

   int16 ReadBigEndianInt16() {
     int16 result = PeekBigEndianInt16();
     RetrieveInt16();
     return result;
   }

   uint16 ReadBigEndianUint16() {
     uint16 result = PeekBigEndianUint16();
     RetrieveInt16();
     return result;
   }

   int16 ReadLittleEndianInt16() {
     int16 result = ReadLittleEndianUint16();
     RetrieveInt16();
     return result;
   }

   uint16 ReadLittleEndianUint16() {
     uint16 result = PeekLittleEndianUint16();
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
   int64 PeekBigEndianInt64() const {
     return static_cast<int64>(PeekBigEndianUint64());
   }

   uint64 PeekBigEndianUint64() const {
     DCHECK(ReadableBytes() >= sizeof(int64));
     uint64 be64 = 0;
     ::memcpy(&be64, Peek(), sizeof(be64));
     return base::NetToHost64(be64);
     return be64;
   }

   int64 PeekLittleEndianInt64() const {
     return static_cast<int64>(PeekLittleEndianUint64());
   }

   uint64 PeekLittleEndianUint64() const {
     DCHECK(ReadableBytes() >= sizeof(int64));
     uint64 be64 = 0;
     ::memcpy(&be64, Peek(), sizeof(be64));
     return base::ByteSwapToLE64(be64);
     return be64;
   }
   

   int32 PeekBigEndianInt32() const {
     return static_cast<int32>(PeekBigEndianUint32());
   }

   uint32 PeekBigEndianUint32() const {
     DCHECK(ReadableBytes() >= sizeof(int32));
     uint32 be32 = 0;
     ::memcpy(&be32, Peek(), sizeof(be32));
     return base::NetToHost32(be32);
     return be32;
   }

   int32 PeekLittleEndianInt32() const {
     return static_cast<int32>(PeekLittleEndianUint32());
   }

   uint32 PeekLittleEndianUint32() const {
     DCHECK(ReadableBytes() >= sizeof(int32));
     uint32 be32 = 0;
     ::memcpy(&be32, Peek(), sizeof(be32));
     return base::ByteSwapToLE32(be32);
     return be32;
   }

   int16 PeekBigEndianInt16() const {
     return static_cast<int16>(PeekBigEndianUint16());
   }

   uint16 PeekBigEndianUint16() const {
     DCHECK(ReadableBytes() >= sizeof(int16));
     uint16 be16 = 0;
     ::memcpy(&be16, Peek(), sizeof(be16));
     return base::NetToHost16(be16);
   }

   int16 PeekLittleEndianInt16() const {
     return static_cast<int16>(PeekLittleEndianUint16());
   }

   uint16 PeekLittleEndianUint16() const {
     DCHECK(ReadableBytes() >= sizeof(int16));
     uint16 be16 = 0;
     ::memcpy(&be16, Peek(), sizeof(be16));
     return base::ByteSwapToLE16(be16);
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
   void PrependBigEndianInt64(int64 x) {
     PrependBigEndianUint64(x);
   }

   void PrependBigEndianUint64(uint64 x) {
     uint64 be64 = base::HostToNet64(x);
     Prepend(&be64, sizeof be64);
   }

   void PrependLittleEndianInt64(int64 x) {
     PrependLittleEndianUint64(x);
   }

   void PrependLittleEndianUint64(uint64 x) {
     uint64 be64 = base::ByteSwapToLE64(x);
     Prepend(&be64, sizeof be64);
   }

   ///
   /// Prepend int32 using network endian
   ///
   void PrependBigEndianInt32(int32 x) {
     PrependBigEndianUint32(x);
   }

   void PrependBigEndianUint32(uint32 x) {
     uint32 be32 = base::HostToNet32(x);
     Prepend(&be32, sizeof be32);
   }

   void PrependLittleEndianInt32(int32 x) {
     PrependLittleEndianUint32(x);
   }

   void PrependLittleEndianUint32(uint32 x) {
     uint32 be32 = base::ByteSwapToLE32(x);
     Prepend(&be32, sizeof be32);
   }

   void PrependBigEndianInt16(int16 x) {
     PrependBigEndianUint16(x);
   }

   void PrependBigEndianUint16(uint16 x) {
     int16 be16 = base::HostToNet16(x);
     Prepend(&be16, sizeof be16);
   }

   void PrependLittleEndianInt16(int16 x) {
     PrependBigEndianUint16(x);
   }

   void PrependLittleEndianUint16(uint16 x) {
     int16 be16 = base::ByteSwapToLE16(x);
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
     other.Write(ToStringPiece());
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
