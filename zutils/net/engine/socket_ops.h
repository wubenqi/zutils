// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//
// Socket api 的简单封装
// 

#ifndef NET_ENGINE_SOCKET_OPS_H_
#define NET_ENGINE_SOCKET_OPS_H_
#pragma once

#include "base/basictypes.h"

#include <string>

#if defined(OS_WIN)
#include <winsock2.h>
const SOCKET kInvalidSocket = INVALID_SOCKET;
const int kSocketError = SOCKET_ERROR;
#elif defined(OS_POSIX)
typedef int SOCKET;
const SOCKET kInvalidSocket = -1;
const int kSocketError = -1;
#endif


namespace net {

enum WaitState {
	NOT_WAITING      = 0,
	WAITING_ACCEPT   = 1,
	WAITING_READ     = 3,
	WAITING_CLOSE    = 4
};

int CreateTCPListeningSocket(const std::string& ip, const std::string& port, bool is_numeric_host_address, SOCKET *listen_socket);

// 非阻塞连接
// 返回值：
// -1：发生错误
//  1：连接成功
//  0：正在连接
int CreateTCPConnectedSocket(const std::string& ip, const std::string& port, bool is_numeric_host_address, SOCKET *connect_socket);

// 创建阻塞连接
bool CreateBlockingTCPConnectedSocket(const std::string& ip, const std::string& port, bool is_numeric_host_address, SOCKET *connect_socket);

int Accept(SOCKET acceptor, SOCKET *accepted_socket);

#if defined(OS_WIN)
std::string FormatLastWSAErrorA(unsigned messageid);
#endif

}

#endif
