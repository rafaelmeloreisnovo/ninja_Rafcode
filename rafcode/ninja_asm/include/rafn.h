/* RAFCODE authorial L0 candidate. Outbound license: TOKEN_VAZIO.
 * GNU-style inline ASM where available; scalar reference elsewhere.
 * No libc/heap/syscall/IO/runtime allocator in this core.
 */
#ifndef RAFN_NINJA_ASM_H
#define RAFN_NINJA_ASM_H

typedef __UINT8_TYPE__ rafn_u8;
typedef __UINT32_TYPE__ rafn_u32;
#define RAFN_LIMIT 32u
#define RAFN_NONE 255u
#define RAFN_UNKNOWN 0u
#define RAFN_READY 1u
#define RAFN_RUNNING 2u
#define RAFN_COMPLETE 3u
#define RAFN_FAILSAFE 4u
#define RAFN_RESTART_APPROVED 0x5241464eu

typedef struct {
  rafn_u32 deps; /* bit i = prerequisite node i */
  rafn_u32 stamp; /* untrusted caller label; NOT a source hash */
} rafn_node;

typedef struct {
  rafn_node pinned[RAFN_LIMIT];
  rafn_u32 done;
  rafn_u32 full_mask;
  rafn_u8 count;
  rafn_u8 state;
  rafn_u8 active;
  rafn_u8 reserved;
} rafn_plan;

/* Synchronous L0 only. An external host must bind files, receipts, clocks,
 * commands, jobserver semantics, transactions, and cryptographic hashes.
 * open() snapshots the graph; later calls fail closed on any node mutation.
 */
int rafn_open(rafn_plan* p, const rafn_node* nodes, rafn_u8 count);
int rafn_next(rafn_plan* p, const rafn_node* nodes, rafn_u8* out_id);
int rafn_ack(rafn_plan* p, const rafn_node* nodes, rafn_u8 id, rafn_u8 success);
int rafn_restart(rafn_plan* p, const rafn_node* nodes, rafn_u32 approval);

/* ISA primitives. first() requires mask != 0; it returns RAFN_NONE if 0. */
rafn_u32 rafn_unmet(rafn_u32 deps, rafn_u32 done);
rafn_u8 rafn_first(rafn_u32 mask);
rafn_u8 rafn_backend(void); /* 0 scalar, 1 ARMv7, 2 AArch64, 3 x86-64 */
#endif
