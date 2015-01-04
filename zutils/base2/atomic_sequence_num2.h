// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef BASE2_ATOMIC_SEQUENCE_NUM2_H_
#define BASE2_ATOMIC_SEQUENCE_NUM2_H_

#include "base/atomicops.h"
#include "base/basictypes.h"

namespace base {

class AtomicSequenceNumbe2 {
public:
	AtomicSequenceNumbe2(Atomic32 value = 0) : seq_(value) { }

  inline void Reset(Atomic32 value) {
    base::subtle::Release_Store(&seq_, value);
  }
	
	inline int GetNext() {
		return static_cast<int>(
			base::subtle::NoBarrier_AtomicIncrement(&seq_, 1) - 1);
	}

private:
	base::subtle::Atomic32 seq_;
	DISALLOW_COPY_AND_ASSIGN(AtomicSequenceNumbe2);
};

}  // namespace base

#endif  // BASE_ATOMIC_SEQUENCE_NUM2_H_
