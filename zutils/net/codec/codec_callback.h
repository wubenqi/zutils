// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_CODEC_CODEC_CALLBACK_H_
#define NET_ENGINE_CODEC_CODEC_CALLBACK_H_

#include "net/engine/io_handler.h"

namespace net {

class CodecCallback {
public:
  virtual ~CodecCallback() {}

  virtual int  OnCodecNewConnection(const IOHandlerPtr& ih) { return 0; }
  virtual int  OnCodecDataReceived(const IOHandlerPtr& ih, const void* data, base::Time receive_time) { return 0; }
  virtual int  OnCodecConnectionClosed(const IOHandlerPtr& ih) { return 0; }
};

}

#endif
