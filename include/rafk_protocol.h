/* Copyright 2026 Rafael Melo Reis
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef RAFK_PROTOCOL_H
#define RAFK_PROTOCOL_H
#include "rafk_types.h"

enum raf_op {
  RAF_OP_VOID      = 0x00u,
  RAF_OP_BOOT      = 0x11u,
  RAF_OP_DECLARE   = 0x23u,
  RAF_OP_RELATE    = 0x35u,
  RAF_OP_ARM       = 0x47u,
  RAF_OP_COMMIT    = 0x59u,
  RAF_OP_OBSERVE   = 0x6bu,
  RAF_OP_ACK       = 0x7du,
  RAF_OP_CANCEL    = 0x8fu,
  RAF_OP_RELEASE   = 0xa1u,
  RAF_OP_RESET     = 0xb3u,
  RAF_OP_HALT      = 0xc5u
};

enum raf_status {
  RAF_STATUS_VOID      = 0u,
  RAF_STATUS_OK        = 1u,
  RAF_STATUS_REJECTED  = 2u,
  RAF_STATUS_CONFLICT  = 3u,
  RAF_STATUS_HALTED    = 4u,
  RAF_STATUS_UNKNOWN   = 255u
};

#define RAF_FLAG_PRESENT       (1u << 0)
#define RAF_FLAG_AUTHORIZED    (1u << 1)
#define RAF_FLAG_REVERSIBLE    (1u << 2)
#define RAF_FLAG_EVIDENCE      (1u << 3)
#define RAF_FLAG_STRICT        (1u << 4)
#define RAF_FLAG_RESERVED_MASK 0xe0u

typedef struct raf_frame {
  raf_u8  op;
  raf_u8  flags;
  raf_u16 lane;
  raf_u32 subject;
  raf_u32 left;
  raf_u32 right;
} raf_frame;

typedef struct raf_state {
  raf_u64 epoch;
  raf_u64 relations;
  raf_u32 active_subject;
  raf_u32 declared_subject;
  raf_u32 last_value;
  raf_u16 lane;
  raf_u8  armed;
  raf_u8  halted;
} raf_state;

typedef struct raf_event {
  raf_u64 stamp;
  raf_u32 subject;
  raf_u32 value;
  raf_u16 lane;
  raf_u8  op;
  raf_u8  status;
} raf_event;

#define RAFK_PROFILE_NO_LIBC       (1ull << 0)
#define RAFK_PROFILE_NO_HEAP       (1ull << 1)
#define RAFK_PROFILE_NO_SYSCALL    (1ull << 2)
#define RAFK_PROFILE_ONE_FRAME     (1ull << 3)
#define RAFK_PROFILE_NO_RECURSION  (1ull << 4)
#define RAFK_PROFILE_NO_CORE_LOOP  (1ull << 5)
#define RAFK_PROFILE_FIXED_FRAMES  (1ull << 6)
#define RAFK_PROFILE_CALLER_OWNED  (1ull << 7)
#define RAFK_PROFILE_V1 (RAFK_PROFILE_NO_LIBC | RAFK_PROFILE_NO_HEAP |   RAFK_PROFILE_NO_SYSCALL | RAFK_PROFILE_ONE_FRAME |   RAFK_PROFILE_NO_RECURSION | RAFK_PROFILE_NO_CORE_LOOP |   RAFK_PROFILE_FIXED_FRAMES | RAFK_PROFILE_CALLER_OWNED)

#ifdef __cplusplus
extern "C" {
#endif
void rafk_step(const raf_frame* frame, raf_state* state, raf_event* event);
raf_u64 rafk_profile(void);
#ifdef __cplusplus
}
#endif

#endif
