/*
 * Copyright 2026 Rafael Melo Reis
 *
 * RAFCODE freestanding L0: explicit void/empty states and flag packing.
 * This file is newly authored code and does not relabel inherited Ninja code.
 */
#ifndef RAFCODE_FREESTANDING_RAF_L0_H_
#define RAFCODE_FREESTANDING_RAF_L0_H_

#if !defined(__clang__) && !defined(__GNUC__)
#error "raf_l0 v1 requires a compiler exposing GNU-style builtin integer type macros"
#endif

#ifndef __UINT8_TYPE__
#error "compiler must define __UINT8_TYPE__"
#endif
#ifndef __UINT32_TYPE__
#error "compiler must define __UINT32_TYPE__"
#endif
#ifndef __UINT64_TYPE__
#error "compiler must define __UINT64_TYPE__"
#endif
#ifndef __SIZE_TYPE__
#error "compiler must define __SIZE_TYPE__"
#endif

typedef __UINT8_TYPE__ raf_u8;
typedef __UINT32_TYPE__ raf_u32;
typedef __UINT64_TYPE__ raf_u64;
typedef __SIZE_TYPE__ raf_usize;

typedef char raf_l0_assert_u8[(sizeof(raf_u8) == 1) ? 1 : -1];
typedef char raf_l0_assert_u32[(sizeof(raf_u32) == 4) ? 1 : -1];
typedef char raf_l0_assert_u64[(sizeof(raf_u64) == 8) ? 1 : -1];

typedef struct raf_l0_view {
  const raf_u8* data;
  raf_usize size;
  raf_u8 provided;
} raf_l0_view;

typedef struct raf_l0_result {
  raf_u64 flags;
  raf_u64 meta;
} raf_l0_result;

#define RAF_L0_STATE_ABSENT   0u
#define RAF_L0_STATE_EMPTY    1u
#define RAF_L0_STATE_NUL      2u
#define RAF_L0_STATE_SPACE    3u
#define RAF_L0_STATE_BYTE     4u
#define RAF_L0_STATE_SEQUENCE 5u
#define RAF_L0_STATE_INVALID  6u

#define RAF_L0_F_PROVIDED     (1ull << 0)
#define RAF_L0_F_VALID        (1ull << 1)
#define RAF_L0_F_ABSENT       (1ull << 2)
#define RAF_L0_F_EMPTY        (1ull << 3)
#define RAF_L0_F_HAS_DATA     (1ull << 4)
#define RAF_L0_F_SIZE_ONE     (1ull << 5)
#define RAF_L0_F_SIZE_MANY    (1ull << 6)
#define RAF_L0_F_FIRST_NUL    (1ull << 7)
#define RAF_L0_F_FIRST_SPACE  (1ull << 8)
#define RAF_L0_F_FIRST_BYTE   (1ull << 9)
#define RAF_L0_F_SEQUENCE     (1ull << 10)
#define RAF_L0_F_INVALID      (1ull << 11)

#define RAF_L0_P_NO_LIBC        (1ull << 32)
#define RAF_L0_P_NO_HEAP        (1ull << 33)
#define RAF_L0_P_NO_SYSCALL     (1ull << 34)
#define RAF_L0_P_NO_RECURSION   (1ull << 35)
#define RAF_L0_P_NO_SOURCE_LOOP (1ull << 36)
#define RAF_L0_P_NO_EXT_RUNTIME (1ull << 37)
#define RAF_L0_P_ZERO_CONTEXT   (1ull << 38)
#define RAF_L0_P_ZERO_ATTENTION (1ull << 39)
#define RAF_L0_P_ZERO_WEIGHTS   (1ull << 40)
#define RAF_L0_PROFILE_V1   (RAF_L0_P_NO_LIBC | RAF_L0_P_NO_HEAP | RAF_L0_P_NO_SYSCALL |    RAF_L0_P_NO_RECURSION | RAF_L0_P_NO_SOURCE_LOOP |    RAF_L0_P_NO_EXT_RUNTIME | RAF_L0_P_ZERO_CONTEXT |    RAF_L0_P_ZERO_ATTENTION | RAF_L0_P_ZERO_WEIGHTS)

#define RAF_L0_META_FIRST_MASK 0xffull
#define RAF_L0_META_STATE_SHIFT 8u
#define RAF_L0_META_STATE_MASK (0xfull << RAF_L0_META_STATE_SHIFT)

#if defined(__GNUC__) || defined(__clang__)
#define RAF_L0_NOINLINE __attribute__((noinline))
#define RAF_L0_SECTION(name) __attribute__((section(name)))
#define RAF_L0_USED __attribute__((used))
#else
#define RAF_L0_NOINLINE
#define RAF_L0_SECTION(name)
#define RAF_L0_USED
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Preconditions: in != 0 and out != 0.
 * data may be 0 when provided=0 or when provided=1,size=0.
 * No bytes are read unless provided=1,size>0,data!=0.
 */
void raf_l0_observe(const raf_l0_view* in, raf_l0_result* out);

#ifdef __cplusplus
}
#endif

#endif
