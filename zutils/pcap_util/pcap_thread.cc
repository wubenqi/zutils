// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "pcap_util/pcap_thread.h"

#include <vector>
#include <iostream>

#include "base/strings/string_number_conversions.h"

void PCapThread::OnDataReceived(const CommChannelInfo& channel, char* data, int data_len, base::Time cap_time) {
  if (delegate_)
    delegate_->OnDataReceived(channel, data, data_len, cap_time);
}

void PacketCaptureCallBack(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data) {
  base::Time now = base::Time::Now();

	PCapThread* cap_thread = (PCapThread*)param;

	int pkt_data_len = header->caplen;

	IPHeader* ip_header;
	unsigned short ip_header_len = 0;
	unsigned short ip_total_len = 0;

	TCPHeader* tcp_header;
	unsigned short tcp_header_len;

	unsigned short source_port;
	int dest_port;

	char* data;
	int data_len;

	ip_header = (IPHeader*)(pkt_data + sizeof(EthernetHeader));
	ip_header_len = ip_header->ip_header_len*4;
	
	if(ip_header->ip_protocol==IPPROTO_TCP) {
		tcp_header = (TCPHeader*)( pkt_data + ip_header_len + sizeof(EthernetHeader) );
		tcp_header_len = tcp_header->data_offset*4;

		source_port=ntohs(tcp_header->source_port);
		dest_port=ntohs(tcp_header->dest_port);

		data = (char*)(pkt_data + sizeof(EthernetHeader) + ip_header_len + tcp_header_len);
		data_len = pkt_data_len - sizeof(EthernetHeader) - ip_header_len - tcp_header_len;

		ip_total_len = ntohs(ip_header->ip_total_length);

		if (data_len>0 && (pkt_data_len == (sizeof(EthernetHeader) + ip_total_len)) && cap_thread!=NULL) {
      CommChannelInfo channel(*(uint32*)&ip_header->ip_srcaddr, source_port, *(uint32*)&ip_header->ip_destaddr, dest_port);
			cap_thread->OnDataReceived(channel, data, data_len, now);
		}
	}
}

PCapThread::PCapThread(const std::string& cap_tcp_port, const std::string& device_name, const std::string& device_descr, u_int net_mask, PCapUtil::Delegate* delegate)
	: name_(device_name)
	, description_(device_descr)
	, net_mask_(net_mask)
	, cap_(NULL)
	, runnging_(false)
  , cap_thread_(this, device_name)
  , delegate_(delegate) {

	packet_filter_ = "tcp port " + cap_tcp_port;
  cap_port_ = base::StringToInt(cap_tcp_port, &cap_port_);
}

bool PCapThread::IsCapPort(int port) {
	return port == cap_port_;
}

bool PCapThread::Create() {
	/* Open the adapter */
	if ((cap_= pcap_open_live(name_.c_str(),	// name of the device
		65536,			// portion of the packet to capture. 
		// 65536 grants that the whole packet will be captured on all the MACs.
		PCAP_OPENFLAG_PROMISCUOUS,			// promiscuous mode (nonzero means promiscuous)
		10,			// read timeout
		err_			// error buffer
		)) == NULL) {
		std::cerr << "Failed to open the adapter." << std::endl;
		return false;
	}

	/* Check the link layer. We support only Ethernet for simplicity. */
	if(pcap_datalink(cap_) != DLT_EN10MB) {
		std::cerr << "Only supports ethernet networks." << std::endl;
		return false;
	}

	//compile the filter
	if (pcap_compile(cap_, &fcode_, packet_filter_.c_str(), 1, net_mask_) <0 ) {
		std::cerr << "Unable to compile the packet filter. Check the syntax." << std::endl;
		return false;
	}

	//set the filter
	if (pcap_setfilter(cap_, &fcode_)<0) {
		std::cerr << "Error setting the filter." << std::endl;
		return false;
	}

	cap_thread_.Start();

	return true;
}

void PCapThread::Destroy() {
	if (runnging_) {
		runnging_ = false;
		cap_thread_.Join();
	}

	if (cap_) {
		pcap_close(cap_);
		cap_= NULL;
	}
}

void PCapThread::Run() {
	runnging_ = true;
	while (runnging_) {
		pcap_dispatch(cap_, 0, PacketCaptureCallBack, (u_char*)this);
	}
}

PCapThread::~PCapThread() {
	Destroy();
}
