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

#include "static_key.hpp"
#include <cstdio>
#include <cstdlib>

static_key_cpp::flag key1;
static_key_cpp::flag key2;

int main() {
  key1.fuse(getenv("KEY1"));
  key2.fuse(getenv("KEY2"));

  if (key1)
    printf("key1 enabled.\n");
  else
    printf("key1 disabled.\n");

  if (key2)
    printf("key2 enabled.\n");
  else
    printf("key2 disabled.\n");

  return EXIT_SUCCESS;
}
