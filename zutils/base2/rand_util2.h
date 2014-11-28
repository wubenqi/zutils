// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef BASE2_RAND_UTIL2_H_
#define BASE2_RAND_UTIL2_H_
#pragma once

#include <list>
#include "base/rand_util.h"

namespace base {

// Returns a random double in range [0, 1). Thread-safe.
float RandFloat();

// 随机浮点数区间(min,max). Thread-safe.
float RandFloat(float min_arg, float max_arg);


/****************************************************************************
 * 区间值随机数生成器
 * 有这样的需求：10%的概率得到A，35%的概率得到B，而55%的概率得到C
 * 换成区间分布就是说从随机数落在(0，10]则返回A，随机数落在(10，10+35]则
 * 返回B，随机数在(10+35，10+35+55]则返回C。
 *
 * std::string A("A");
 * std::string B("B");
 * std::string C("C");
 * IntervalRandom<std::string> myrand;
 * myrand.AddInterval(A, 10);
 * myrand.AddInterval(B, 35);
 * myrand.AddInterval(C, 55);
 * std::string value = myrand.Generate();
 *
 ****************************************************************************/
template <class T>
class IntervalRandom {
private:
    struct IntervalRandom_Itv_Map {
        uint32 _value;
        T _ret;
    };
    uint32 m_max_value;
    std::list<IntervalRandom_Itv_Map> m_intervals;
    //boost::mt19937 m_engine;
public:
    IntervalRandom() {
        m_max_value = 0;
    }

    // 添加一个区间
    void AddInterval(T t, uint32 value) {
        if (value == 0)
            return;
        m_max_value += value;
        IntervalRandom_Itv_Map item;
        item._value = m_max_value;
        item._ret = t;
        m_intervals.push_back(item);
    }
    
    // 根据概率找到对应的值
    T Generate() {
        T ret;
        //uint32 value = 1 + (uint32)(m_max_value * (rand() / (RAND_MAX + 1.0)));

		uint32 value = static_cast<uint32>(RandInt(1, m_max_value));
        //boost::uniform_int<uint32> distribution(1, m_max_value);
        //uint32 value = distribution(m_engine);
        
		typename std::list<IntervalRandom_Itv_Map>::iterator itr;
        for (itr = m_intervals.begin(); itr != m_intervals.end(); ++itr) {
            if (value <= itr->_value) {
                ret = itr->_ret;
                break;
            }
        }
        return ret;
    }

    void Reset() {
        m_max_value = 0;
        m_intervals.clear();
    }

    inline bool HasInterval() const { return m_max_value > 0; }
};

}  // namespace base

#endif  // BASE_RAND_UTIL2_H_
