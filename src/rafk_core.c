/* Copyright 2026 Rafael Melo Reis
 * SPDX-License-Identifier: Apache-2.0
 */
#include "rafk_protocol.h"

RAF_ALWAYS_INLINE void raf_emit(const raf_frame* f, const raf_state* s,
                                raf_event* e, raf_u8 status, raf_u32 value) {
  e->stamp = s->epoch;
  e->subject = f->subject;
  e->value = value;
  e->lane = f->lane;
  e->op = f->op;
  e->status = status;
}

RAF_ALWAYS_INLINE raf_u8 raf_basic_gate(const raf_frame* f) {
  return (raf_u8)(((f->flags & RAF_FLAG_RESERVED_MASK) == 0u) &&
                  ((f->flags & RAF_FLAG_PRESENT) != 0u));
}

RAF_NOINLINE RAF_USED RAF_SECTION(".rafcode.text.core")
void rafk_step(const raf_frame* f, raf_state* s, raf_event* e) {
  raf_u8 ok;
  raf_u32 mixed;

  if (s->halted != 0u) {
    raf_emit(f, s, e, RAF_STATUS_HALTED, 0u);
    return;
  }

  ok = raf_basic_gate(f);
  if (f->op == RAF_OP_VOID) {
    raf_emit(f, s, e, RAF_STATUS_VOID, 0u);
    return;
  }
  if (ok == 0u) {
    raf_emit(f, s, e, RAF_STATUS_REJECTED, 0u);
    return;
  }

  mixed = f->left ^ ((f->right << 7u) | (f->right >> 25u)) ^ f->subject;

  switch (f->op) {
    case RAF_OP_BOOT:
      s->epoch += 1u;
      s->lane = f->lane;
      s->armed = 0u;
      raf_emit(f, s, e, RAF_STATUS_OK, mixed);
      return;
    case RAF_OP_DECLARE:
      s->declared_subject = f->subject;
      s->last_value = mixed;
      s->epoch += 1u;
      raf_emit(f, s, e, RAF_STATUS_OK, mixed);
      return;
    case RAF_OP_RELATE:
      s->relations ^= (((raf_u64)f->subject << 32u) | mixed);
      s->epoch += 1u;
      raf_emit(f, s, e, RAF_STATUS_OK, mixed);
      return;
    case RAF_OP_ARM:
      s->active_subject = f->subject;
      s->armed = (raf_u8)((f->flags & RAF_FLAG_AUTHORIZED) != 0u);
      s->epoch += 1u;
      raf_emit(f, s, e, s->armed ? RAF_STATUS_OK : RAF_STATUS_REJECTED, mixed);
      return;
    case RAF_OP_COMMIT:
      if (s->armed == 0u || s->active_subject != f->subject) {
        raf_emit(f, s, e, RAF_STATUS_CONFLICT, mixed);
        return;
      }
      s->last_value = mixed;
      s->armed = 0u;
      s->epoch += 1u;
      raf_emit(f, s, e, RAF_STATUS_OK, mixed);
      return;
    case RAF_OP_OBSERVE:
      raf_emit(f, s, e, RAF_STATUS_OK, s->last_value);
      return;
    case RAF_OP_ACK:
      s->epoch += (raf_u64)((f->flags & RAF_FLAG_EVIDENCE) != 0u);
      raf_emit(f, s, e, RAF_STATUS_OK, mixed);
      return;
    case RAF_OP_CANCEL:
      s->armed = 0u;
      s->active_subject = 0u;
      s->epoch += 1u;
      raf_emit(f, s, e, RAF_STATUS_OK, 0u);
      return;
    case RAF_OP_RELEASE:
      if (s->declared_subject == f->subject) s->declared_subject = 0u;
      s->epoch += 1u;
      raf_emit(f, s, e, RAF_STATUS_OK, 0u);
      return;
    case RAF_OP_RESET:
      s->relations = 0u;
      s->active_subject = 0u;
      s->declared_subject = 0u;
      s->last_value = 0u;
      s->lane = 0u;
      s->armed = 0u;
      s->epoch += 1u;
      raf_emit(f, s, e, RAF_STATUS_OK, 0u);
      return;
    case RAF_OP_HALT:
      s->halted = 1u;
      s->epoch += 1u;
      raf_emit(f, s, e, RAF_STATUS_HALTED, 0u);
      return;
    default:
      raf_emit(f, s, e, RAF_STATUS_UNKNOWN, mixed);
      return;
  }
}

RAF_NOINLINE RAF_USED RAF_SECTION(".rafcode.text.meta")
raf_u64 rafk_profile(void) {
  return RAFK_PROFILE_V1;
}
