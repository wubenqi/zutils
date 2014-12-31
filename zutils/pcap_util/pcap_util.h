// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef PCAP_UTIL_PCAP_UTIL_H_
#define PCAP_UTIL_PCAP_UTIL_H_

#include <vector>

#include "base/time/time.h"

#include "pcap_util/comm_channel_info.h"

class PCapThread;
class PCapUtil {
public:
  class Delegate {
  public:
    virtual ~Delegate() {}
    virtual void OnDataReceived(const CommChannelInfo& channel, const char* data, uint32 data_len, base::Time cap_time) { }
  };

  PCapUtil(Delegate* delegate) 
    : delegate_(delegate) {}

  ~PCapUtil() {}

  bool StartPCap(const std::string& cap_tcp_port);
  void ShutdowPCap();

private:
  typedef std::vector<PCapThread*> PCapThreadList;
  PCapThreadList cap_threads_;

  Delegate* delegate_;
};

#endif
