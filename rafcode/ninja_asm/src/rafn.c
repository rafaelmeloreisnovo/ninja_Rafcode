#include "rafn.h"

#if !defined(RAFN_FORCE_SCALAR) && defined(__aarch64__)
#define RAFN_ISA 2u
#elif !defined(RAFN_FORCE_SCALAR) && defined(__arm__) && defined(__ARM_ARCH) && (__ARM_ARCH >= 7)
#define RAFN_ISA 1u
#elif !defined(RAFN_FORCE_SCALAR) && defined(__x86_64__)
#define RAFN_ISA 3u
#else
#define RAFN_ISA 0u
#endif

rafn_u8 rafn_backend(void) { return (rafn_u8)RAFN_ISA; }

rafn_u32 rafn_unmet(rafn_u32 deps, rafn_u32 done) {
#if RAFN_ISA == 2u
  rafn_u32 x;
  __asm__("bic %w0, %w1, %w2" : "=&r"(x) : "r"(deps), "r"(done));
  return x;
#elif RAFN_ISA == 1u
  rafn_u32 x;
  __asm__("bic %0, %1, %2" : "=&r"(x) : "r"(deps), "r"(done));
  return x;
#elif RAFN_ISA == 3u
  rafn_u32 x;
  __asm__("movl %1, %0\n\tnotl %0\n\tandl %2, %0"
          : "=&r"(x) : "r"(done), "r"(deps) : "cc");
  return x;
#else
  return deps & ~done;
#endif
}

rafn_u8 rafn_first(rafn_u32 mask) {
  rafn_u32 x;
  if (!mask) return RAFN_NONE;
#if RAFN_ISA == 2u
  __asm__("rbit %w0, %w1\n\tclz %w0, %w0" : "=&r"(x) : "r"(mask));
#elif RAFN_ISA == 1u
  __asm__("rbit %0, %1\n\tclz %0, %0" : "=&r"(x) : "r"(mask));
#elif RAFN_ISA == 3u
  __asm__("bsfl %1, %0" : "=r"(x) : "r"(mask) : "cc");
#else
  for (x=0u; x<32u && (mask & ((rafn_u32)1u<<x))==0u; ++x) { }
#endif
  return (rafn_u8)x;
}

static rafn_u32 mask_for(rafn_u8 n) {
  return n==32u ? 0xffffffffu : (((rafn_u32)1u<<n)-1u);
}

static int same(const rafn_plan* p, const rafn_node* nodes) {
  rafn_u8 i;
  if (!p || !nodes || p->count==0u || p->count>RAFN_LIMIT) return 0;
  for (i=0u; i<p->count; ++i)
    if (p->pinned[i].deps!=nodes[i].deps ||
        p->pinned[i].stamp!=nodes[i].stamp) return 0;
  return 1;
}

static int valid_dag(const rafn_node* nodes, rafn_u8 n, rafn_u32 all) {
  rafn_u8 i, pass;
  rafn_u32 visited=0u;
  for (i=0u; i<n; ++i) {
    rafn_u32 bit=(rafn_u32)1u << i;
    if ((nodes[i].deps & ~all) || (nodes[i].deps & bit)) return 0;
  }
  for (pass=0u; pass<n; ++pass) {
    rafn_u32 before=visited;
    for (i=0u; i<n; ++i) {
      rafn_u32 bit=(rafn_u32)1u << i;
      if (!(visited & bit) && !rafn_unmet(nodes[i].deps,visited))
        visited |= bit;
    }
    if (visited==all) return 1;
    if (visited==before) return 0;
  }
  return visited==all;
}

int rafn_open(rafn_plan* p, const rafn_node* nodes, rafn_u8 n) {
  rafn_u8 i;
  if (!p) return 0;
  p->state=RAFN_UNKNOWN; p->active=RAFN_NONE;
  p->done=0u; p->full_mask=0u; p->count=0u; p->reserved=0u;
  if (!nodes || n==0u || n>RAFN_LIMIT) return 0;
  if (!valid_dag(nodes,n,mask_for(n))) return 0;
  for (i=0u; i<n; ++i) p->pinned[i]=nodes[i];
  p->count=n; p->full_mask=mask_for(n); p->state=RAFN_READY;
  return 1;
}

int rafn_next(rafn_plan* p, const rafn_node* nodes, rafn_u8* out_id) {
  rafn_u32 pending;
  if (!out_id) return 0;
  *out_id=RAFN_NONE;
  if (!p || p->state!=RAFN_READY) return 0;
  if (!same(p,nodes)) {p->state=RAFN_FAILSAFE;return 0;}
  pending=rafn_unmet(p->full_mask,p->done);
  if (!pending) {p->state=RAFN_COMPLETE;return 0;}
  while (pending) {
    rafn_u8 id=rafn_first(pending);
    rafn_u32 bit=(rafn_u32)1u<<id;
    if (!rafn_unmet(p->pinned[id].deps,p->done)) {
      p->active=id; p->state=RAFN_RUNNING; *out_id=id; return 1;
    }
    pending &= ~bit;
  }
  p->state=RAFN_FAILSAFE;
  return 0;
}

int rafn_ack(rafn_plan* p, const rafn_node* nodes, rafn_u8 id, rafn_u8 success) {
  if (!p || p->state!=RAFN_RUNNING || id!=p->active || success>1u) return 0;
  if (!same(p,nodes)) {p->state=RAFN_FAILSAFE;p->active=RAFN_NONE;return 0;}
  if (!success) {p->state=RAFN_FAILSAFE;p->active=RAFN_NONE;return 0;}
  p->done |= (rafn_u32)1u<<id;
  p->active=RAFN_NONE; p->state=RAFN_READY;
  return 1;
}

int rafn_restart(rafn_plan* p, const rafn_node* nodes, rafn_u32 approval) {
  if (!p || approval!=RAFN_RESTART_APPROVED ||
      (p->state!=RAFN_FAILSAFE && p->state!=RAFN_COMPLETE) || !same(p,nodes)) return 0;
  p->done=0u; p->active=RAFN_NONE; p->state=RAFN_READY;
  return 1;
}
