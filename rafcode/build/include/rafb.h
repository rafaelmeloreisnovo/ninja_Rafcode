/* RAFCODE build gate v1 - scoped implementation candidate.
 * Outbound license: TOKEN_VAZIO. No implicit license grant.
 * Freestanding C, fixed capacity, no system calls, allocation, I/O, or libc.
 * This schedules commands; it does NOT execute files, compilers or APK signing.
 */
#ifndef RAF_BUILD_GATE_H
#define RAF_BUILD_GATE_H

typedef __UINT8_TYPE__  rafb_u8;
typedef __UINT16_TYPE__ rafb_u16;
typedef __UINT32_TYPE__ rafb_u32;

#define RAFB_LIMIT 8u
#define RAFB_ANY_ABI 0xffu
#define RAFB_NONE 0xffu
#define RAFB_PRIMARY 0u
#define RAFB_SPARE 1u
#define RAFB_REQUIRED 0x0001u
#define RAFB_ALLOW_ROLLBACK 0x0002u
#define RAFB_ALLOW_FAILOVER 0x0004u
#define RAFB_ALLOW_FAILBACK 0x0008u
#define RAFB_NO_CLAIM 0x8000u
#define RAFB_VALID_FLAGS 0x800fu

#define RAFB_READY 1u
#define RAFB_RUNNING 2u
#define RAFB_COMPLETE 3u
#define RAFB_FAILSAFE 4u
#define RAFB_ROLLED_BACK 5u
#define RAFB_UNKNOWN 0xfeu

/* ABI PROFILE, not proof of ABI linkage or runtime. */
#define RAFB_ANDROID_ARMV7 0u
#define RAFB_ANDROID_AARCH64 1u
#define RAFB_ANDROID_X86 2u
#define RAFB_ANDROID_X86_64 3u
#define RAFB_BARE_ARMV7M 4u
#define RAFB_BARE_AARCH64 5u
#define RAFB_BARE_RV32 6u
#define RAFB_BARE_RV64 7u

/* One module = one source-file command contract. A spare has no own step
 * in the ordinary graph, and may only run after its primary fails.
 * id 0..7 selects a bit. deps is a mask of PRIMARY ids only.
 */
typedef struct {
  rafb_u8 id;
  rafb_u8 abi;
  rafb_u8 role;
  rafb_u8 fallback; /* id of spare, or NONE */
  rafb_u16 required_flags;
  rafb_u16 reserved; /* MUST be zero; unknown bits fail closed */
  rafb_u32 deps;
  rafb_u32 budget_ticks; /* 0 is invalid, relative to external monotonic tick */
  rafb_u32 source_stamp; /* byte identity provided by caller; not a hash proof */
} rafb_module;

typedef struct {
  rafb_u32 done;
  rafb_u32 faults;
  rafb_u32 start_tick;
  rafb_u32 checkpoint;
  rafb_u32 event_code; /* 0 unknown, 1 good, 2 fail, 3 timeout, 4 rolled back */
  rafb_u16 flags;
  rafb_u8 count;
  rafb_u8 abi;
  rafb_u8 state;
  rafb_u8 active;  /* module ID or NONE */
  rafb_u8 recover; /* primary ID or NONE */
  rafb_u8 pending; /* spare ID or NONE */
  rafb_u8 failback_authorized;
  rafb_module pinned[RAFB_LIMIT]; /* immutable by-value snapshot, no heap */
} rafb_gate;

/* All functions return 1=success, 0=blocked; no undefined "empty is 0".
 * Caller must provide a stable modules array for the whole run.
 */
int rafb_open(rafb_gate* g, const rafb_module* modules,
              rafb_u8 count, rafb_u8 abi, rafb_u16 flags);
int rafb_next(rafb_gate* g, const rafb_module* modules,
              rafb_u32 now, rafb_u8* module_id);
int rafb_ack(rafb_gate* g, const rafb_module* modules,
             rafb_u8 module_id, rafb_u8 success);
int rafb_watchdog(rafb_gate* g, const rafb_module* modules, rafb_u32 now);
int rafb_rollback(rafb_gate* g, rafb_u16 explicit_flags);
int rafb_failback(rafb_gate* g, rafb_u16 explicit_flags);
#endif
