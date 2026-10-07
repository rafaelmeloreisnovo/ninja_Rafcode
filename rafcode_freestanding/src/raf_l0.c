/*
 * Copyright 2026 Rafael Melo Reis
 * SPDX-License-Identifier: Apache-2.0
 */
#include "raf_l0.h"

#if defined(__clang__) || defined(__GNUC__)
#define RAF_L0_ALWAYS_INLINE static inline __attribute__((always_inline))
#else
#define RAF_L0_ALWAYS_INLINE static inline
#endif

RAF_L0_ALWAYS_INLINE void raf_l0_emit(raf_l0_result* out,
                                      raf_u64 flags,
                                      raf_u8 first,
                                      raf_u32 state) {
  out->flags = RAF_L0_PROFILE_V1 | flags;
  out->meta = (raf_u64)first |
      (((raf_u64)state & 0xfull) << RAF_L0_META_STATE_SHIFT);
}

RAF_L0_NOINLINE RAF_L0_USED RAF_L0_SECTION(".rafcode.text.l0")
void raf_l0_observe(const raf_l0_view* in, raf_l0_result* out) {
  raf_u8 first;
  raf_u64 first_flag;

  if (in->provided == 0u) {
    if (in->size == (raf_usize)0u && in->data == (const raf_u8*)0) {
      raf_l0_emit(out, RAF_L0_F_VALID | RAF_L0_F_ABSENT, 0u,
                  RAF_L0_STATE_ABSENT);
      return;
    }
    raf_l0_emit(out, RAF_L0_F_INVALID, 0u, RAF_L0_STATE_INVALID);
    return;
  }

  if (in->size == (raf_usize)0u) {
    raf_l0_emit(out,
                RAF_L0_F_PROVIDED | RAF_L0_F_VALID | RAF_L0_F_EMPTY,
                0u, RAF_L0_STATE_EMPTY);
    return;
  }

  if (in->data == (const raf_u8*)0) {
    raf_l0_emit(out, RAF_L0_F_PROVIDED | RAF_L0_F_INVALID, 0u,
                RAF_L0_STATE_INVALID);
    return;
  }

  first = in->data[0];
  if (first == 0x00u) {
    first_flag = RAF_L0_F_FIRST_NUL;
  } else if (first == 0x20u) {
    first_flag = RAF_L0_F_FIRST_SPACE;
  } else {
    first_flag = RAF_L0_F_FIRST_BYTE;
  }

  if (in->size > (raf_usize)1u) {
    raf_l0_emit(out,
                RAF_L0_F_PROVIDED | RAF_L0_F_VALID | RAF_L0_F_HAS_DATA |
                    RAF_L0_F_SIZE_MANY | RAF_L0_F_SEQUENCE | first_flag,
                first, RAF_L0_STATE_SEQUENCE);
    return;
  }

  if (first == 0x00u) {
    raf_l0_emit(out,
                RAF_L0_F_PROVIDED | RAF_L0_F_VALID | RAF_L0_F_HAS_DATA |
                    RAF_L0_F_SIZE_ONE | RAF_L0_F_FIRST_NUL,
                first, RAF_L0_STATE_NUL);
    return;
  }

  if (first == 0x20u) {
    raf_l0_emit(out,
                RAF_L0_F_PROVIDED | RAF_L0_F_VALID | RAF_L0_F_HAS_DATA |
                    RAF_L0_F_SIZE_ONE | RAF_L0_F_FIRST_SPACE,
                first, RAF_L0_STATE_SPACE);
    return;
  }

  raf_l0_emit(out,
              RAF_L0_F_PROVIDED | RAF_L0_F_VALID | RAF_L0_F_HAS_DATA |
                  RAF_L0_F_SIZE_ONE | RAF_L0_F_FIRST_BYTE,
              first, RAF_L0_STATE_BYTE);
}
