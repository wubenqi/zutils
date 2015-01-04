// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_HTTP_HTTP_REQUEST_CODEC_H_
#define NET_HTTP_HTTP_REQUEST_CODEC_H_

#include "net/engine/io_handler.h"
#include "net/http/http_server_request_info.h"
#include "net/http/http_server_response_info.h"

namespace net {

class HttpRequestCodec : public IOHandler::Delegate {
public:
  class Delegate {
  public:
    virtual ~Delegate() {}

    virtual int OnHttpConnection(const IOHandlerPtr& ih) { return 0; }
    virtual int OnHttpRequest(const IOHandlerPtr& ih, const HttpServerRequestInfo& request, base::Time receive_time) { return 0; }
    virtual int OnHttpConnectionClosed(const IOHandlerPtr& ih) { return 0; }
  };

  HttpRequestCodec(HttpRequestCodec::Delegate* delegate) 
    : delegate_(delegate) {
  }

  virtual ~HttpRequestCodec() {}

  // From IOHandler::Delegate
  virtual int  OnNewConnection(const IOHandlerPtr& ih);
  virtual int  OnDataReceived(const IOHandlerPtr& ih, IOBuffer* data, base::Time receive_time);
  virtual int  OnConnectionClosed(const IOHandlerPtr& ih);

  void SendResponse(const IOHandlerPtr& ih, const HttpServerResponseInfo& response);
  void Send404(const IOHandlerPtr& ih);
  void Send500(const IOHandlerPtr& ih, const std::string& message);

protected:
  HttpRequestCodec::Delegate* delegate_;
};

}

#endif
