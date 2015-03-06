// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/codec/length_header/length_header_codec.h"

namespace net {

int LengthHeaderCodec::OnDataReceived(const IOHandlerPtr& ih, IOBuffer* data, base::Time receive_time) {
  int ret = 0;
  while (data->ReadableBytes() >= header_len_) {
    int32 len = data->PeekBigEndianInt32();
    if (len > 65536 || len < 0) {
      LOG(ERROR) << "Invalid length " << len;
      ret = -1;
      break;
    } else if (data->ReadableBytes() >= len + header_len_) {
      data->Retrieve(header_len_);
      base::StringPiece message(data->Peek(), len);
      ret = delegate_->OnLengthHeaderDataReceived(ih, message, receive_time);
      data->Retrieve(len);

      if (ret == -1) {
        break;
      }
    } else {
      break;
    }
  }

  return ret;
}

}
