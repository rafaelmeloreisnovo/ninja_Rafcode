#include "rafb_sha3.h"
static const rafb_u64 rc[24] = {
  0x0000000000000001ULL,0x0000000000008082ULL,0x800000000000808aULL,
  0x8000000080008000ULL,0x000000000000808bULL,0x0000000080000001ULL,
  0x8000000080008081ULL,0x8000000000008009ULL,0x000000000000008aULL,
  0x0000000000000088ULL,0x0000000080008009ULL,0x000000008000000aULL,
  0x000000008000808bULL,0x800000000000008bULL,0x8000000000008089ULL,
  0x8000000000008003ULL,0x8000000000008002ULL,0x8000000000000080ULL,
  0x000000000000800aULL,0x800000008000000aULL,0x8000000080008081ULL,
  0x8000000000008080ULL,0x0000000080000001ULL,0x8000000080008008ULL
};
static const rafb_u8 rho[25] = {
   0, 1,62,28,27,36,44, 6,55,20, 3,10,43,25,39,41,45,15,21, 8,18, 2,61,56,14
};
/* 32-bit rotations avoid ARM32 __aeabi_llsl/llsr. */
static rafb_u64 rol(rafb_u64 v, rafb_u32 shift) {
  rafb_u32 lo=(rafb_u32)v, hi=(rafb_u32)(v>>32u), a,b;
  if (shift==0u) return v;
  if (shift==32u) { a=hi;b=lo; }
  else if (shift<32u) {
    a=(lo<<shift)|(hi>>(32u-shift));
    b=(hi<<shift)|(lo>>(32u-shift));
  } else {
    shift-=32u;
    a=(hi<<shift)|(lo>>(32u-shift));
    b=(lo<<shift)|(hi>>(32u-shift));
  }
  return ((rafb_u64)b<<32u)|(rafb_u64)a;
}
static const rafb_u8 pi[25]={
  0,10,20,5,15,16,1,11,21,6,7,17,2,12,22,23,8,18,3,13,14,24,9,19,4
};
static void keccak(rafb_u64 a[25]) {
  rafb_u32 round,x,y;
  for (round=0u;round<24u;++round) {
    rafb_u64 c[5],d[5],b[25];
    for (x=0u;x<5u;++x)
      c[x]=a[x]^a[x+5u]^a[x+10u]^a[x+15u]^a[x+20u];
    for (x=0u;x<5u;++x) d[x]=c[x==0u?4u:x-1u]^rol(c[x==4u?0u:x+1u],1u);
    for (y=0u;y<5u;++y)
      for (x=0u;x<5u;++x) a[x+5u*y]^=d[x];
    for (y=0u;y<5u;++y)
      for (x=0u;x<5u;++x)
        b[pi[x+5u*y]] = rol(a[x+5u*y],rho[x+5u*y]);
    for (y=0u;y<5u;++y)
      for (x=0u;x<5u;++x)
        a[x+5u*y] = b[x+5u*y] ^
          ((~b[(x==4u?0u:x+1u)+5u*y]) & b[(x>=3u?x-3u:x+2u)+5u*y]);
    a[0]^=rc[round];
  }
}
static void absorb(rafb_sha3 *ctx) {
  rafb_u32 j;
  for (j=0u;j<RAFB_SHA3_RATE/8u;++j) {
    const rafb_u8 *p=ctx->buffer+j*8u;
    rafb_u32 lo=(rafb_u32)p[0]|((rafb_u32)p[1]<<8u)|
      ((rafb_u32)p[2]<<16u)|((rafb_u32)p[3]<<24u);
    rafb_u32 hi=(rafb_u32)p[4]|((rafb_u32)p[5]<<8u)|
      ((rafb_u32)p[6]<<16u)|((rafb_u32)p[7]<<24u);
    ctx->lane[j] ^= ((rafb_u64)hi<<32u)|(rafb_u64)lo;
  }
  keccak(ctx->lane);
  ctx->used=0u;
}
void rafb_sha3_init(rafb_sha3 *ctx) {
  rafb_u32 i;
  if (!ctx) return;
  for(i=0u;i<25u;++i) ctx->lane[i]=0u;
  for(i=0u;i<RAFB_SHA3_RATE;++i) ctx->buffer[i]=0u;
  ctx->used=0u;ctx->status=RAFB_SHA3_ACTIVE;
}
int rafb_sha3_update(rafb_sha3 *ctx, const rafb_u8 *bytes, rafb_u32 size) {
  rafb_u32 i;
  if (!ctx || ctx->status!=RAFB_SHA3_ACTIVE || (size && !bytes) ||
      ctx->used>=RAFB_SHA3_RATE) {
    if(ctx) ctx->status=RAFB_SHA3_ERROR;
    return 0;
  }
  for(i=0u;i<size;++i) {
    ctx->buffer[ctx->used++]=bytes[i];
    if(ctx->used==RAFB_SHA3_RATE) absorb(ctx);
  }
  return 1;
}
int rafb_sha3_final(rafb_sha3 *ctx, rafb_u8 out[RAFB_SHA3_DIGEST]) {
  rafb_u32 i;
  if (!ctx || ctx->status!=RAFB_SHA3_ACTIVE || !out ||
      ctx->used>=RAFB_SHA3_RATE) {
    if(ctx) ctx->status=RAFB_SHA3_ERROR;
    return 0;
  }
  for (i=ctx->used;i<RAFB_SHA3_RATE;++i) ctx->buffer[i]=0u;
  ctx->buffer[ctx->used] ^= 0x06u;
  ctx->buffer[RAFB_SHA3_RATE-1u] ^= 0x80u;
  absorb(ctx);
  for(i=0u;i<4u;++i) {
    rafb_u64 lane=ctx->lane[i];
    rafb_u32 lo=(rafb_u32)lane,hi=(rafb_u32)(lane>>32u);
    out[i*8u+0u]=(rafb_u8)lo;
    out[i*8u+1u]=(rafb_u8)(lo>>8u);
    out[i*8u+2u]=(rafb_u8)(lo>>16u);
    out[i*8u+3u]=(rafb_u8)(lo>>24u);
    out[i*8u+4u]=(rafb_u8)hi;
    out[i*8u+5u]=(rafb_u8)(hi>>8u);
    out[i*8u+6u]=(rafb_u8)(hi>>16u);
    out[i*8u+7u]=(rafb_u8)(hi>>24u);
  }
  ctx->status=RAFB_SHA3_FINISHED;
  return 1;
}
int rafb_sha3_equal(const rafb_u8 *actual, const rafb_u8 *expected) {
  rafb_u8 delta=0u;rafb_u32 i;
  if(!actual || !expected) return 0;
  for(i=0u;i<RAFB_SHA3_DIGEST;++i)
    delta=(rafb_u8)(delta | (rafb_u8)(actual[i]^expected[i]));
  return delta==0u;
}
