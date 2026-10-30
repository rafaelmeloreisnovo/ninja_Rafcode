/* RAFCODE freestanding build graph/gate. Nothing here calls a hosted API. */
#include "rafb.h"

static rafb_u32 bit(rafb_u8 id) { return (rafb_u32)1u << id; }
static const rafb_module* get(const rafb_module* m, rafb_u8 n, rafb_u8 id) {
  rafb_u8 i;
  for (i = 0u; i < n; ++i) if (m[i].id == id) return &m[i];
  return (const rafb_module*)0;
}
static int fault(rafb_gate* g, const rafb_module* m, rafb_u32 cause) {
  const rafb_module* p = get(m, g->count, g->active);
  const rafb_module* b;
  if (!p) { g->state = RAFB_FAILSAFE; return 0; }
  g->faults |= bit(p->id);
  g->event_code = cause;
  if (g->recover != RAFB_NONE) { /* second failure is terminal */
    g->state = RAFB_FAILSAFE; g->active = RAFB_NONE; return 0;
  }
  if ((g->flags & RAFB_ALLOW_FAILOVER) && p->fallback != RAFB_NONE) {
    b = get(m, g->count, p->fallback);
    if (b && b->role == RAFB_SPARE && b->deps == p->deps &&
        (b->required_flags & g->flags) == b->required_flags &&
        (b->abi == g->abi || b->abi == RAFB_ANY_ABI)) {
      g->recover = p->id;
      g->pending = b->id;
      g->active = RAFB_NONE;
      g->state = RAFB_READY;
      return 1; /* failover is planned, not proved successful */
    }
  }
  g->active = RAFB_NONE;
  g->state = RAFB_FAILSAFE;
  return 0;
}
int rafb_open(rafb_gate* g, const rafb_module* m,
              rafb_u8 n, rafb_u8 abi, rafb_u16 flags) {
  rafb_u8 i, j; rafb_u32 seen=0u, primaries=0u, resolved=0u;
  if (!g) return 0;
  g->state = RAFB_UNKNOWN; g->active = RAFB_NONE;
  g->done=0u; g->faults=0u; g->checkpoint=0u; g->start_tick=0u;
  g->event_code=0u; g->count=n; g->abi=abi; g->flags=flags;
  g->recover=RAFB_NONE; g->pending=RAFB_NONE; g->failback_authorized=0u;
  if (!m || n==0u || n>RAFB_LIMIT || abi>RAFB_BARE_RV64 ||
      (flags & RAFB_NO_CLAIM)==0u) return 0;
  for (i=0u; i<n; ++i) {
    if (m[i].id>=RAFB_LIMIT || (seen & bit(m[i].id)) || m[i].reserved ||
        m[i].budget_ticks==0u || m[i].role>RAFB_SPARE ||
        (m[i].abi!=abi && m[i].abi!=RAFB_ANY_ABI) ||
        (m[i].required_flags & flags)!=m[i].required_flags ||
        m[i].deps & ~0xffu || m[i].deps & bit(m[i].id)) return 0;
    seen |= bit(m[i].id);
    if (m[i].role==RAFB_PRIMARY) primaries |= bit(m[i].id);
    else if (m[i].fallback!=RAFB_NONE) return 0;
  }
  if (primaries==0u) return 0;
  for (i=0u; i<n; ++i) {
    const rafb_module* b;
    if (m[i].deps & ~primaries) return 0;
    if (m[i].role!=RAFB_PRIMARY || m[i].fallback==RAFB_NONE) continue;
    b=get(m,n,m[i].fallback);
    if (!b || b->role!=RAFB_SPARE || b->deps!=m[i].deps ||
        b->id==m[i].id) return 0;
    for (j=0u; j<n; ++j)
      if (i!=j && m[j].role==RAFB_PRIMARY && m[j].fallback==b->id) return 0;
  }
  /* Kahn-style acyclicity check for <=8 primary IDs; no recursion/heap. */
  for (j=0u; j<n; ++j)
    for (i=0u; i<n; ++i)
      if (m[i].role==RAFB_PRIMARY && !(resolved & bit(m[i].id)) &&
          !(m[i].deps & ~resolved)) resolved |= bit(m[i].id);
  if (resolved != primaries) return 0;
  g->state = RAFB_READY;
  return 1;
}
int rafb_next(rafb_gate* g, const rafb_module* m,
              rafb_u32 now, rafb_u8* module_id) {
  rafb_u8 i; rafb_u32 all=0u;
  if (!g || !m || !module_id ||
      (g->state!=RAFB_READY && g->state!=RAFB_RUNNING)) return 0;
  *module_id=RAFB_NONE;
  if (g->state==RAFB_RUNNING) return 0; /* caller must ack/watchdog */
  if (g->pending!=RAFB_NONE) {
    const rafb_module* b=get(m,g->count,g->pending);
    if (!b || (b->deps & ~g->done)) {g->state=RAFB_FAILSAFE;return 0;}
    g->active=b->id; g->pending=RAFB_NONE;
    g->start_tick=now; g->state=RAFB_RUNNING;
    *module_id=b->id; return 1;
  }
  for (i=0u; i<g->count; ++i) if (m[i].role==RAFB_PRIMARY) all |= bit(m[i].id);
  if ((g->done & all)==all) {g->state=RAFB_COMPLETE;g->event_code=1u;return 0;}
  for (i=0u; i<g->count; ++i) {
    if (m[i].role!=RAFB_PRIMARY || (g->done & bit(m[i].id)) ||
        (m[i].deps & ~g->done)) continue;
    g->active=m[i].id;g->start_tick=now;g->state=RAFB_RUNNING;
    *module_id=m[i].id;return 1;
  }
  g->state=RAFB_FAILSAFE;return 0;
}
int rafb_ack(rafb_gate* g, const rafb_module* m,
             rafb_u8 id, rafb_u8 success) {
  if (!g || !m || g->state!=RAFB_RUNNING || id!=g->active || success>1u) return 0;
  if (!success) return fault(g,m,2u);
  g->done |= bit(id);
  if (g->recover!=RAFB_NONE) {g->done |= bit(g->recover);g->recover=RAFB_NONE;}
  g->active=RAFB_NONE; g->state=RAFB_READY; g->event_code=1u;
  g->checkpoint=g->done; return 1;
}
int rafb_watchdog(rafb_gate* g, const rafb_module* m, rafb_u32 now) {
  const rafb_module* p;
  if (!g || !m || g->state!=RAFB_RUNNING) return 0;
  p=get(m,g->count,g->active);
  if (!p) {g->state=RAFB_FAILSAFE;return 0;}
  /* External monotonic ticks; elapsed < 2^31 is caller invariant. */
  if ((rafb_u32)(now-g->start_tick)<=p->budget_ticks) return 1;
  return fault(g,m,3u);
}
int rafb_rollback(rafb_gate* g, rafb_u16 flags) {
  if (!g || !(flags & RAFB_ALLOW_ROLLBACK) ||
      !(g->flags & RAFB_ALLOW_ROLLBACK)) return 0;
  /* Logical checkpoint only. External generated files require host rollback. */
  g->done=g->checkpoint;g->active=RAFB_NONE;g->pending=RAFB_NONE;
  g->recover=RAFB_NONE;g->state=RAFB_ROLLED_BACK;g->event_code=4u;
  return 1;
}
int rafb_failback(rafb_gate* g, rafb_u16 flags) {
  if (!g || g->state!=RAFB_ROLLED_BACK ||
      !(flags & RAFB_ALLOW_FAILBACK) || !(g->flags & RAFB_ALLOW_FAILBACK)) return 0;
  /* Explicitly authorized NEW run; no implicit recovery of host artifacts. */
  g->done=0u;g->checkpoint=0u;g->faults=0u;g->state=RAFB_READY;
  g->event_code=0u;g->failback_authorized=1u;return 1;
}
