// SPDX-License-Identifier: MIT
//
// MIT License
//
// Copyright (c) 2024 Yingwei Zheng
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include "static_key.h"

namespace static_key_cpp {

class flag {
  static_key m_key;

public:
  flag() = default;
  ~flag() = default;
  flag(const flag &) = delete;
  flag(flag &&) = delete;
  flag &operator=(const flag &) = delete;
  flag &operator=(flag &&) = delete;

  void fuse(bool enabled) { static_key_fuse(&m_key, enabled); }
  operator bool() const {
    return static_key_branch(const_cast<static_key *>(&m_key));
  }
};

} // namespace static_key_cpp
