/* RAFCODE authorial implementation of FIPS 202 SHA3-256 (public algorithm).
 * Freestanding, no libc, malloc, syscalls, device dependencies.
 * This is a hash, NOT an APK signature, MAC, source provenance or encryption. */
#ifndef RAFB_SHA3_H
#define RAFB_SHA3_H
#include "rafb.h"
typedef __UINT64_TYPE__ rafb_u64;
#define RAFB_SHA3_RATE 136u
#define RAFB_SHA3_DIGEST 32u
#define RAFB_SHA3_ACTIVE 1u
#define RAFB_SHA3_FINISHED 2u
#define RAFB_SHA3_ERROR 3u
typedef struct {
  rafb_u64 lane[25];
  rafb_u8 buffer[RAFB_SHA3_RATE];
  rafb_u32 used;
  rafb_u8 status;
} rafb_sha3;
void rafb_sha3_init(rafb_sha3 *ctx);
int rafb_sha3_update(rafb_sha3 *ctx, const rafb_u8 *bytes, rafb_u32 size);
int rafb_sha3_final(rafb_sha3 *ctx, rafb_u8 out[RAFB_SHA3_DIGEST]);
/* Caller must authorize the expected digest through an independent trusted channel. */
int rafb_sha3_equal(const rafb_u8 *actual, const rafb_u8 *expected);
#endif
