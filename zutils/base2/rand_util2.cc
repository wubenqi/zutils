// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "base2/rand_util2.h"

#include <math.h>

#include <limits>

#include "base/basictypes.h"
#include "base/logging.h"

namespace base {

float RandFloat() {
	// We try to get maximum precision by masking out as many bits as will fit
	// in the target type's mantissa, and raising it to an appropriate power to
	// produce output in the range [0, 1).  For IEEE 754 doubles, the mantissa
	// is expected to accommodate 53 bits.

	COMPILE_ASSERT(std::numeric_limits<float>::radix == 2, otherwise_use_scalbn);
	static const int kBits = std::numeric_limits<float>::digits;
	uint64 random_bits = base::RandUint64() & ((GG_UINT64_C(1) << kBits) - 1);
	float result = ldexp(static_cast<float>(random_bits), -1 * kBits);
	DCHECK(result >= 0.0f && result < 1.0f);
	return result;

}

float RandFloat(float min_arg, float max_arg) {
	return 0.0f;
}

}  // namespace base
