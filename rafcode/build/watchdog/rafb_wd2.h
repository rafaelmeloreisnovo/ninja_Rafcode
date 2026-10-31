/* RAFCODE dual watchdog L0. Copyright (c) 2026 Rafael Melo Reis.
 * Outbound license: TOKEN_VAZIO. Pure C freestanding, caller-owned memory.
 * No threads, timer, syscalls, malloc, or hardware reset. */
#ifndef RAFB_WD2_H
#define RAFB_WD2_H
#include "rafb.h"
#define RAFB_WD2_ACTIVE 1u
#define RAFB_WD2_ISOLATED 2u
#define RAFB_WD2_HALTED 3u
#define RAFB_WD2_EVENT_NONE 0u
#define RAFB_WD2_EVENT_WORKER_STALE 1u
#define RAFB_WD2_EVENT_GUARD_STALE 2u
#define RAFB_WD2_EVENT_TAMPER 3u
#define RAFB_WD2_EVENT_CLOCK 4u
#define RAFB_WD2_EVENT_ATTESTATION 5u
#define RAFB_WD2_EVENT_REARM 6u
#define RAFB_WD2_REARM_EXPLICIT 0xa51eu
#define RAFB_WD2_MAGIC 0x5744324cu

typedef struct {
  rafb_u32 magic;
  rafb_u32 source_stamp;
  rafb_u32 worker_budget;
  rafb_u32 guard_budget;
  rafb_u32 config_seal; /* cheap corruption guard, NOT authentication */
  rafb_u32 worker_tick;
  rafb_u32 guard_tick;
  rafb_u32 last_tick;
  rafb_u32 worker_seq;
  rafb_u32 guard_seq;
  rafb_u32 last_guard_worker_seq;
  rafb_u32 epoch;
  rafb_u8 state;
  rafb_u8 event;
  rafb_u16 reserved;
} rafb_wd2;

/* Caller certifies monotonic tick and independent scheduling of supervisor.
 * At init source_stamp=0 is invalid, not a placeholder for unknown. */
int rafb_wd2_open(rafb_wd2 *s, rafb_u32 source_stamp,
                  rafb_u32 worker_budget, rafb_u32 guard_budget, rafb_u32 tick);
int rafb_wd2_worker_progress(rafb_wd2 *s, rafb_u32 seq, rafb_u32 tick);
/* Guard1's receipt must match current worker progress, and itself advance. */
int rafb_wd2_guard1_receipt(rafb_wd2 *s, rafb_u32 observed_worker_seq,
                            rafb_u32 guard_seq, rafb_u32 tick);
/* Guard2 MUST be called independently from Guard1 or a hung Guard1 can block it.
 * FAILSAFE is latched; no automatic restart/failback, no claim of HW watchdog. */
int rafb_wd2_guard2_poll(rafb_wd2 *s, rafb_u32 tick);
int rafb_wd2_rearm(rafb_wd2 *s, rafb_u32 source_stamp,
                   rafb_u32 explicit_authority, rafb_u32 tick);
#endif
