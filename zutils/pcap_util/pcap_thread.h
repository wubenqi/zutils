// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef PCAP_UTIL_PCAP_THREAD_H_
#define PCAP_UTIL_PCAP_THREAD_H_

#include <string>
#include <map>

#include "base/basictypes.h"
#include "base/threading/simple_thread.h"
#include "base/time/time.h"

#define HAVE_REMOTE
#include "pcap.h"

#include "pcap_util/net_headers.h"
#include "pcap_util/comm_channel_info.h"
#include "pcap_util/pcap_util.h"

class PCapThread : public base::DelegateSimpleThread::Delegate {
public:
  PCapThread(const std::string& cap_tcp_port, const std::string& device_name, const std::string& device_descr, u_int net_mask, PCapUtil::Delegate* delegate);
	virtual ~PCapThread();

	bool Create();
	void Destroy();
	bool IsCapPort(int port);
	
  void OnDataReceived(const CommChannelInfo& channel, char* data, int data_len, base::Time cap_time);

protected:
	virtual void Run();

private:
	std::string packet_filter_;
	std::string name_;
	std::string description_;
	u_int net_mask_;
	pcap_t* cap_;
	char err_[PCAP_ERRBUF_SIZE];
	bool runnging_;
	struct bpf_program fcode_;
	int cap_port_;

  base::DelegateSimpleThread cap_thread_;
  PCapUtil::Delegate* delegate_;
}; 

#endif // PCAP_UTIL_PCAP_THREAD_H_

