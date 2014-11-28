// Copyright (c) 2006-2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE2_NULL_LOCK_H_
#define BASE2_NULL_LOCK_H_
#pragma once

class NullLock {
 public:
  NullLock() {}
  ~NullLock() {}
  void Acquire() {}
  void Release() {}
  bool Try() {}
  // Null implementation if not debug.
  void AssertAcquired() const {}

private:
  DISALLOW_COPY_AND_ASSIGN(Lock);
};

#endif  // BASE_LOCK_H_
