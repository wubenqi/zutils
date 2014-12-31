// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "pcap_util/pcap_util.h"

#include <vector>
#include <iostream>

#include "base/logging.h"

#include "pcap_util/pcap_thread.h"

struct PCapDevice {
  std::string name;
  std::string description;
  u_int net_mask;
};

typedef std::vector<PCapDevice> PCapDeviceList;

void EnumerateDevices(PCapDeviceList* device_list) {
	pcap_if_t *alldevs;
	pcap_if_t *d;
	char err_buf[PCAP_ERRBUF_SIZE];

	if(pcap_findalldevs(&alldevs, err_buf) == -1) {
		LOG(ERROR)<< "No network adapters detected!";
	} else {
		PCapDevice device;
		for(d=alldevs; d; d=d->next) {
			if(d->name) {
				device.name = d->name;
				if (d->description) {
					device.description = d->description;
				} else {
					device.description = d->name;
				}
				if(d->addresses != NULL) {
					/* Retrieve the mask of the first address of the interface */
					device.net_mask=((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
				} else {	/* If the interface is without addresses we suppose to be in a C class network */
					device.net_mask=0xffffff; 
				}
				device_list->push_back(device);
			}
		}

		pcap_freealldevs(alldevs);
	}
}

bool PCapUtil::StartPCap(const std::string& cap_tcp_port) {
  PCapDeviceList device_list;
  EnumerateDevices(&device_list);

  if (device_list.empty()) {
    std::cerr << "Can't not find device" << std::endl; 
    return false;
  }

  for (size_t i=0; i<device_list.size(); ++i) {
    std::cout << device_list[i].description << " ==> " << device_list[i].name << std::endl;
    PCapThread* cap_thread = new PCapThread(cap_tcp_port, device_list[i].name, device_list[i].description, device_list[i].net_mask, delegate_);
    cap_threads_.push_back(cap_thread);
  }

  for (size_t i=0; i<cap_threads_.size(); ++i) {
    cap_threads_[i]->Create();
  }

  return true;
}

void PCapUtil::ShutdowPCap() {
  for (size_t i=0; i<cap_threads_.size(); ++i) {
    cap_threads_[i]->Destroy();
  }

  cap_threads_.clear();
}
