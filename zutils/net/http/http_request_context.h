// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_HTTP_HTTP_REQUEST_CONTEXT_H_
#define NET_HTTP_HTTP_REQUEST_CONTEXT_H_

#include "net/base/io_buffer.h"
#include "net/http/http_server_request_info.h"
#include "net/third_party/http_parser/http_parser.h"

namespace net {

class HttpRequestContext {
public:
  HttpRequestContext();
  ~HttpRequestContext();

  const HttpServerRequestInfo& http_request() const {
    return http_request_;
  }

  void Reset();

  
  int OnHttpDataParse(IOBuffer* data);

  static int OnUrl(http_parser* parser, const char *at, size_t length);
  static int OnHeaderField(http_parser* parser, const char *at, size_t length);
  static int OnHeaderValue(http_parser* parser, const char *at, size_t length);
  static int OnHeadersComplete(http_parser* parser);
  static int OnBody(http_parser* parser, const char *at, size_t length);
  static int OnMessageComplete(http_parser* parser);

private:
  int DoUrl(const char *at, size_t length);
  int DoHeaderField(const char *at, size_t length);
  int DoHeaderValue(const char *at, size_t length);
  int DoHeadersComplete();
  int DoBody(const char *at, size_t length);
  int DoMessageComplete();

  http_parser http_parser_;
  http_parser_settings http_parser_settings_;

  HttpServerRequestInfo http_request_;

  std::string tmp_header_field_;
  bool done_;
};

}

#endif
