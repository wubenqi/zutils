// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/http/http_request_codec.h"

#include "net/http/http_request_context.h"

namespace net {

int HttpRequestCodec::OnNewConnection(const IOHandlerPtr& ih) {
  ih->SetIOContext(new HttpRequestContext());
  delegate_->OnHttpConnection(ih);
  // LOG(INFO) << "HttpRequestCodec::OnNewConnection()";
  return 0;
}

int HttpRequestCodec::OnDataReceived(const IOHandlerPtr& ih, IOBuffer* data, base::Time receive_time) {
  HttpRequestContext* ctx = reinterpret_cast<HttpRequestContext*>(ih->GetIOContext());
  int r = ctx->OnHttpDataParse(data);

  if (0 == r) {
    r = delegate_->OnHttpRequest(ih, ctx->http_request(), receive_time);
  }

  return r;

#if 0
  int ret = 0;
  while (data->ReadableBytes() >= kHeaderLen) {
    int32 len = data->PeekInt32();
    if (len > 65536 || len < 0) {
      LOG(ERROR) << "Invalid length " << len;
      ret = -1;
      break;
    } else if (data->ReadableBytes() >= len + kHeaderLen) {
      data->Retrieve(kHeaderLen);
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
#endif
  return 0;
}

int HttpRequestCodec::OnConnectionClosed(const IOHandlerPtr& ih) {
  // LOG(INFO) << "HttpRequestCodec::OnConnectionClosed()";
  HttpRequestContext* ctx = reinterpret_cast<HttpRequestContext*>(ih->ReleaseIOContext());
  delete ctx;
  delegate_->OnHttpConnectionClosed(ih);
  return 0;
}

void HttpRequestCodec::SendResponse(const IOHandlerPtr& ih, const HttpServerResponseInfo& response) {
  ih->SendInternal2(response.Serialize());
}

void HttpRequestCodec::Send404(const IOHandlerPtr& ih) {
  SendResponse(ih, HttpServerResponseInfo::CreateFor404());
}

void HttpRequestCodec::Send500(const IOHandlerPtr& ih, const std::string& message) {
  SendResponse(ih, HttpServerResponseInfo::CreateFor500(message));
}

}
