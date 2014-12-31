// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef PCAP_UTIL_COMM_CHANNEL_INFO_H_
#define PCAP_UTIL_COMM_CHANNEL_INFO_H_

#include "base/basictypes.h"

struct CommChannelInfo {
  CommChannelInfo(uint32 s_ip, uint16 s_port, uint32 d_ip, uint16 d_port)
    : source_ip(s_ip)
    , source_port(s_port)
    , dest_ip(d_ip)
    , dest_port(d_port) {
  }

  CommChannelInfo(const CommChannelInfo& that)
    : source_ip(that.source_ip)
    , source_port(that.source_port)
    , dest_ip(that.dest_ip)
    , dest_port(that.dest_port) {
  }

  CommChannelInfo& operator=(const CommChannelInfo& that) {
    source_ip = that.source_ip;
    source_port = that.source_port;
    dest_ip = that.dest_ip;
    dest_port = that.dest_port;
    return *this;
  }

  bool operator==(const CommChannelInfo& that) const {
    return source_ip == that.source_ip
      && source_port == that.source_port
      && dest_ip == that.dest_ip
      && dest_port == that.dest_port;
  }

  bool operator!=(const CommChannelInfo& that) const {
    return source_ip != that.source_ip
      || source_port != that.source_port
      || dest_ip != that.dest_ip
      || dest_port != that.dest_port;
  }

  // Required for some STL containers and operations
  bool operator<(const CommChannelInfo& that) const {
    if (source_ip < that.source_ip
      && source_port < that.source_port
      && dest_ip < that.dest_ip
      && dest_port < that.dest_port ) {
        return true;
    }

    return false;
  }

  //////////////////////////////////////////////////////////////////////////
  uint32 source_ip;
  uint16 source_port;
  uint32 dest_ip;
  uint16 dest_port;
};


#endif // PCAP_UTIL_COMM_CHANNEL_INFO_H_

