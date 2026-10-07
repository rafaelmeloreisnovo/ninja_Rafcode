/* Copyright 2026 Rafael Melo Reis
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef RAFK_TYPES_H
#define RAFK_TYPES_H

#ifndef __UINT8_TYPE__
#error compiler_missing_uint8_type
#endif
#ifndef __UINT16_TYPE__
#error compiler_missing_uint16_type
#endif
#ifndef __UINT32_TYPE__
#error compiler_missing_uint32_type
#endif
#ifndef __UINT64_TYPE__
#error compiler_missing_uint64_type
#endif

typedef __UINT8_TYPE__  raf_u8;
typedef __UINT16_TYPE__ raf_u16;
typedef __UINT32_TYPE__ raf_u32;
typedef __UINT64_TYPE__ raf_u64;

typedef char raf_assert_u8[(sizeof(raf_u8) == 1) ? 1 : -1];
typedef char raf_assert_u16[(sizeof(raf_u16) == 2) ? 1 : -1];
typedef char raf_assert_u32[(sizeof(raf_u32) == 4) ? 1 : -1];
typedef char raf_assert_u64[(sizeof(raf_u64) == 8) ? 1 : -1];

#if defined(__clang__) || defined(__GNUC__)
#define RAF_ALWAYS_INLINE static inline __attribute__((always_inline))
#define RAF_NOINLINE __attribute__((noinline))
#define RAF_USED __attribute__((used))
#define RAF_SECTION(x) __attribute__((section(x)))
#else
#define RAF_ALWAYS_INLINE static inline
#define RAF_NOINLINE
#define RAF_USED
#define RAF_SECTION(x)
#endif

#endif
