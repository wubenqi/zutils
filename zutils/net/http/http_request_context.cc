// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/http/http_request_context.h"

#include "base/stl_util.h"
#include "base/logging.h"

namespace net {

HttpRequestContext::HttpRequestContext() {
  memset(&http_parser_settings_, 0, sizeof(http_parser_settings_));
  http_parser_settings_.on_url = OnUrl;
  http_parser_settings_.on_header_field = OnHeaderField;
  http_parser_settings_.on_header_value = OnHeaderValue;
  http_parser_settings_.on_headers_complete = OnHeadersComplete;
  http_parser_settings_.on_body = OnBody;
  http_parser_settings_.on_message_complete = OnMessageComplete;

  http_parser_.data = this;
  http_parser_init(&http_parser_, HTTP_REQUEST);
}

HttpRequestContext::~HttpRequestContext() {
  // LOG(INFO) << "HttpRequestContext::~HttpRequestContext()";
}


void HttpRequestContext::Reset() {
  http_parser_init(&http_parser_, HTTP_REQUEST);
}

int HttpRequestContext::OnHttpDataParse(IOBuffer* data) {
  int n = http_parser_execute(&http_parser_, &http_parser_settings_, data->Peek(), data->ReadableBytes());
  if (n>0) {
    data->Retrieve(n);
  } else {
    return -1;
  }

  if (done_) {
    if (http_parser_.method == 'G') {
      http_request_.method = "GET";
    } else {
      http_request_.method = "GET";
    }
    return 0;
  }
  return 1;
}


int HttpRequestContext::OnUrl(http_parser* parser, const char *at, size_t length) {
  HttpRequestContext* context = reinterpret_cast<HttpRequestContext*>(parser->data);
  return context->DoUrl(at, length);
}

int HttpRequestContext::OnHeaderField(http_parser* parser, const char *at, size_t length) {
  HttpRequestContext* context = reinterpret_cast<HttpRequestContext*>(parser->data);
  return context->DoHeaderField(at, length);
}

int HttpRequestContext::OnHeaderValue(http_parser* parser, const char *at, size_t length) {
  HttpRequestContext* context = reinterpret_cast<HttpRequestContext*>(parser->data);
  return context->DoHeaderValue(at, length);
}

int HttpRequestContext::OnHeadersComplete(http_parser* parser) {
  HttpRequestContext* context = reinterpret_cast<HttpRequestContext*>(parser->data);
  return context->DoHeadersComplete();
}

int HttpRequestContext::OnBody(http_parser* parser, const char *at, size_t length) {
  HttpRequestContext* context = reinterpret_cast<HttpRequestContext*>(parser->data);
  return context->DoBody(at, length);
}

int HttpRequestContext::OnMessageComplete(http_parser* parser) {
  HttpRequestContext* context = reinterpret_cast<HttpRequestContext*>(parser->data);
  return context->DoMessageComplete();
}

int HttpRequestContext::DoUrl(const char *at, size_t length) {
  http_request_.path.assign(at, length);
  return 0;
}

int HttpRequestContext::DoHeaderField(const char *at, size_t length) {
  tmp_header_field_.assign(at, length);
  return 0;
}

int HttpRequestContext::DoHeaderValue(const char *at, size_t length) {
  http_request_.headers.insert(std::make_pair(tmp_header_field_, std::string(at, length)));
  STLClearObject(&tmp_header_field_);
  return 0;
}

int HttpRequestContext::DoHeadersComplete() {
  return 0;
}

int HttpRequestContext::DoBody(const char *at, size_t length) {
  return 0;
}

int HttpRequestContext::DoMessageComplete() {
  done_ = true;
  return 0;
}


}
