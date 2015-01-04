// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_TIME_SYNCOR_H_
#define NET_TIME_SYNCOR_H_

#include <string>
#include "base/basictypes.h"

namespace net {

/*
	TimeSyncor time_syncor;
	…
	if (TimeSyncor:: kMaxTimSamples == time_syncor.OnRecvTimeSample(sent_time, current_time)) {
	  最终同步的时钟= 当前时钟+time_syncor.GetCorrectedDelta();
	}
 */
// 时间同步器
class TimeSyncor {
public:
	static const size_t kMaxTimSamples = 10;

	TimeSyncor() {
		first_delta_flag_ = false;
	}

	// sent_time: 客户端发送给服务器的时间
	// current_time: 服务器返回给客户端的时间
	// return: 返回值为共循环了多少次
	size_t OnRecvTimeSample(uint32 sent_time, uint32 current_time) {
		
		int delta = (current_time-sent_time) / 2;
		if (!first_delta_flag_) {
			first_delta_flag_ = true;
			// 设置系统时间或记录数据
			// delta_time_
			return 0;
		} else {
			deltas_.push_back(delta);
			return deltas_.size();
		}
	}

	// 最终算出的平均值
	uint32 GetCorrectedDelta() {
		DCHECK(deltas_.size() > 2);

		// 排序
		std::sort(deltas_.begin(), deltas_.end());

		// 取中值
		int median = deltas_[deltas_.size()/2];

		int sum = 0;
		int count = 0;
		for (size_t i=0; i<deltas_.size(); ++i) {
			// 抛弃大于中值1.5倍的数值
			if (deltas_[i] < 1.5*median) {
				sum += deltas_[i];
				count++;
			}
		}

		// 使用算术平均得到一个平均值
		return sum / count;
	}


private:
	// 基准时间
	uint32 delta_time_;
	// 所有时间戳（除了第一个）
	std::vector<int> deltas_;
	// 接收到第一个时间戳标记
	bool first_delta_flag_;
};

}

#endif
