// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef BASE2_SCOPED_LOCK_H_
#define BASE2_SCOPED_LOCK_H_
#pragma once

#include "base/synchronization/lock.h"

namespace base {

// A helper class that acquires the given Lock while the ScopedLock is in scope.
class ScopedLock {
public:
	explicit ScopedLock(Lock& lock) : lock_(lock) {
		lock_.Acquire();
	}

	~ScopedLock() {
		lock_.AssertAcquired();
		lock_.Release();
	}

private:
	Lock& lock_;
	DISALLOW_COPY_AND_ASSIGN(ScopedLock);
};

// ScopedUnlock is a helper that will Release() the |lock| argument in the
// constructor, and re-Acquire() it in the destructor.
class ScopedUnlock {
public:
	explicit ScopedUnlock(Lock& lock) : lock_(lock) {
		// We require our caller to have the lock.
		lock_.AssertAcquired();
		lock_.Release();
	}

	~ScopedUnlock() {
		lock_.Acquire();
	}

private:
	Lock& lock_;
	DISALLOW_COPY_AND_ASSIGN(ScopedUnlock);
};

}

#endif

