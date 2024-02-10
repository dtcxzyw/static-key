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
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#ifdef __cplusplus
#include <atomic>
#else
#include <stdatomic.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__OPTIMIZE__) || !defined(__unix__)
#define STATIC_KEY_FORCE_FALLBACK
#endif

#ifdef STATIC_KEY_FORCE_FALLBACK
#define STATIC_KEY_FALLBACK
#else
#if defined(__i386__) || defined(__x86_64__)
#define STATIC_KEY_X86
#define STATIC_KEY_WORDSIZE 2
#define STATIC_KEY_JUMP_BEGIN ""
#define STATIC_KEY_JUMP " jmp "
#define STATIC_KEY_JUMP_END ""
#define STATIC_KEY_C0 " %c0 "
#elif defined(__arm__) || defined(__thumb__)
#define STATIC_KEY_ARM
#define STATIC_KEY_WORDSIZE 4
#define STATIC_KEY_JUMP_BEGIN ""
#define STATIC_KEY_JUMP " b "
#define STATIC_KEY_JUMP_END ""
#define STATIC_KEY_C0 " %c0 "
#elif defined(__riscv)
#define STATIC_KEY_RISCV
#define STATIC_KEY_WORDSIZE 4
#define STATIC_KEY_JUMP_BEGIN                                                  \
  ".option push\n"                                                             \
  ".option norelax\n"                                                          \
  ".option norvc\n"
#define STATIC_KEY_JUMP " j "
#define STATIC_KEY_JUMP_END ".option pop\n"
#define STATIC_KEY_C0 " %0 "
#else
#ifdef STATIC_KEY_ALLOW_FALLBACK
#define STATIC_KEY_FALLBACK
#define STATIC_KEY_WORDSIZE __SIZEOF_POINTER__
#else
#error "Unsupported architecture"
#endif
#endif

#if __SIZEOF_POINTER__ / STATIC_KEY_WORDSIZE == 4
#define STATIC_KEY_ASM_PTR ".quad "
#elif __SIZEOF_POINTER__ / STATIC_KEY_WORDSIZE == 2
#define STATIC_KEY_ASM_PTR ".dword "
#elif __SIZEOF_POINTER__ / STATIC_KEY_WORDSIZE == 1
#define STATIC_KEY_ASM_PTR ".word "
#else
#error "Unknown pointer size"
#endif

#endif

#ifndef NDEBUG
#define STATIC_KEY_DEBUG
#endif

#ifdef __cplusplus
#define STATIC_KEY_ATOMIC_FLAG std::atomic_flag
#define STATIC_KEY_ATOMIC_BOOL std::atomic_bool
#else
#define STATIC_KEY_ATOMIC_FLAG atomic_flag
#define STATIC_KEY_ATOMIC_BOOL atomic_bool
#endif

struct static_key {
#ifdef STATIC_KEY_FALLBACK
  STATIC_KEY_ATOMIC_BOOL enabled;
#endif
#ifdef STATIC_KEY_DEBUG
  STATIC_KEY_ATOMIC_FLAG inited;
#endif
  char padding[2];
};

struct static_key_use {
  int32_t beg;
  int32_t end;
  ptrdiff_t key;
};

__always_inline bool static_key_branch(struct static_key *key) {
#ifdef STATIC_KEY_DEBUG
  assert(atomic_flag_test_and_set(&key->inited) && "Use before init");
#endif
#ifdef STATIC_KEY_FALLBACK
  return atomic_load(&key->enabled);
#else
  __asm__ goto(".p2align 2\n"
               "1:" STATIC_KEY_JUMP_BEGIN STATIC_KEY_JUMP
               " %l[ret_false]\n" STATIC_KEY_JUMP_END "2:\n"
               ".pushsection __static_key_jump_table, \"a\"\n"
               ".p2align 2\n"
               ".long 1b - .\n"
               ".long 2b - . + 4\n" STATIC_KEY_ASM_PTR STATIC_KEY_C0
               " - . + 8\n"
               ".popsection\n"
               :
               : "i"(key)
               :
               : ret_false);
  return true;
ret_false:
  return false;
#endif
}

#ifndef STATIC_KEY_FALLBACK
static inline void static_key_patch_nop(char *begin, char *end) {
  size_t page_size = sysconf(_SC_PAGE_SIZE);
  intptr_t mask = page_size - 1;
  char *page_begin = (char *)(((intptr_t)begin) & ~mask);
  char *page_end = (char *)((((intptr_t)end) + mask) & ~mask);
  ptrdiff_t size = page_end - page_begin;
  mprotect(page_begin, size, PROT_READ | PROT_WRITE | PROT_EXEC);
#if defined(STATIC_KEY_X86)
  for (; begin != end; ++begin)
    *begin = 0x90;
#elif defined(STATIC_KEY_RISCV)
  assert((end - begin) % 4 == 0);
  for (; begin != end; begin += 4)
    *(int32_t *)begin = 0x00000013;
#elif defined(STATIC_KEY_ARM)
  for (; begin != end; ++begin)
    *begin = 0;
#else
#error "Unsupported architecture"
#endif
  mprotect(page_begin, size, PROT_READ | PROT_EXEC);
}
#endif

extern const struct static_key_use __start___static_key_jump_table[];
extern const struct static_key_use __stop___static_key_jump_table[];

static inline void static_key_fuse(struct static_key *entry, bool enabled) {
#ifdef STATIC_KEY_DEBUG
  assert(!atomic_flag_test_and_set(&entry->inited) && "Cannot init twice");
#endif
#ifdef STATIC_KEY_FALLBACK
  atomic_store(&entry->enabled, enabled);
#else
  if (!enabled)
    return;
  const struct static_key_use *iter = __start___static_key_jump_table;
  const struct static_key_use *end = __stop___static_key_jump_table;
  for (; iter != end; ++iter) {
    if (((char *)iter) + iter->key == (char *)entry) {
      static_key_patch_nop(((char *)(iter)) + iter->beg,
                           ((char *)(iter)) + iter->end);
#ifndef STATIC_KEY_X86
      __builtin___clear_cache(((char *)(iter)) + iter->beg,
                              ((char *)(iter)) + iter->end);
#endif
    }
  }
#endif
}

#ifdef __cplusplus
}
#endif
