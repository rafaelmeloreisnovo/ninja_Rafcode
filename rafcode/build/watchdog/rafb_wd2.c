#include "rafb_wd2.h"
static rafb_u32 seal(const rafb_wd2 *s) {
  return s->source_stamp ^ s->worker_budget ^ (s->guard_budget << 1u) ^
         RAFB_WD2_MAGIC ^ 0x9e3779b9u;
}
static int checked(rafb_wd2 *s) {
  if (!s || s->magic != RAFB_WD2_MAGIC || !s->source_stamp ||
      !s->worker_budget || !s->guard_budget ||
      s->worker_budget >= 0x80000000u || s->guard_budget >= 0x80000000u ||
      s->config_seal != seal(s) || s->reserved ||
      (s->state!=RAFB_WD2_ACTIVE && s->state!=RAFB_WD2_ISOLATED &&
       s->state!=RAFB_WD2_HALTED)) {
    if (s) {s->state=RAFB_WD2_HALTED;s->event=RAFB_WD2_EVENT_TAMPER;}
    return 0;
  }
  return 1;
}
static int tick_ok(rafb_wd2 *s, rafb_u32 tick) {
  if ((rafb_u32)(tick-s->last_tick) >= 0x80000000u) {
    s->state=RAFB_WD2_HALTED;s->event=RAFB_WD2_EVENT_CLOCK;return 0;
  }
  s->last_tick=tick;return 1;
}
static int active_tick(rafb_wd2 *s, rafb_u32 tick) {
  return checked(s) && s->state==RAFB_WD2_ACTIVE && tick_ok(s,tick);
}
int rafb_wd2_open(rafb_wd2 *s, rafb_u32 source_stamp,
                  rafb_u32 worker_budget, rafb_u32 guard_budget, rafb_u32 tick) {
  if (!s || !source_stamp || !worker_budget || !guard_budget ||
      worker_budget>=0x80000000u || guard_budget>=0x80000000u) {
    if (s) { s->state=RAFB_WD2_HALTED; s->event=RAFB_WD2_EVENT_TAMPER; }
    return 0;
  }
  s->magic=RAFB_WD2_MAGIC;
  s->source_stamp=source_stamp;
  s->worker_budget=worker_budget;
  s->guard_budget=guard_budget;
  s->config_seal=seal(s);
  s->worker_tick=tick;s->guard_tick=tick;s->last_tick=tick;
  s->worker_seq=0;s->guard_seq=0;s->last_guard_worker_seq=0;s->epoch=1;
  s->state=RAFB_WD2_ACTIVE;s->event=RAFB_WD2_EVENT_NONE;s->reserved=0;
  return 1;
}
int rafb_wd2_worker_progress(rafb_wd2 *s, rafb_u32 seq, rafb_u32 tick) {
  if (!active_tick(s,tick)) return 0;
  if (!seq || seq<=s->worker_seq || (rafb_u32)(tick-s->worker_tick)>s->worker_budget) {
    s->state=RAFB_WD2_ISOLATED;s->event=RAFB_WD2_EVENT_WORKER_STALE;return 0;
  }
  s->worker_seq=seq;s->worker_tick=tick;return 1;
}
int rafb_wd2_guard1_receipt(rafb_wd2 *s, rafb_u32 observed_worker_seq,
                            rafb_u32 guard_seq, rafb_u32 tick) {
  if (!active_tick(s,tick)) return 0;
  if ((rafb_u32)(tick-s->guard_tick)>s->guard_budget) {
    s->state=RAFB_WD2_HALTED;s->event=RAFB_WD2_EVENT_GUARD_STALE;return 0;
  }
  if (observed_worker_seq!=s->worker_seq || !observed_worker_seq ||
      observed_worker_seq<=s->last_guard_worker_seq ||
      !guard_seq || guard_seq<=s->guard_seq) {
    s->state=RAFB_WD2_HALTED;s->event=RAFB_WD2_EVENT_ATTESTATION;return 0;
  }
  if ((rafb_u32)(tick-s->worker_tick)>s->worker_budget) {
    s->state=RAFB_WD2_ISOLATED;s->event=RAFB_WD2_EVENT_WORKER_STALE;return 0;
  }
  s->guard_seq=guard_seq;s->last_guard_worker_seq=observed_worker_seq;
  s->guard_tick=tick;return 1;
}
int rafb_wd2_guard2_poll(rafb_wd2 *s, rafb_u32 tick) {
  if (!active_tick(s,tick)) return 0;
  if ((rafb_u32)(tick-s->guard_tick)>s->guard_budget) {
    s->state=RAFB_WD2_HALTED;s->event=RAFB_WD2_EVENT_GUARD_STALE;return 0;
  }
  if ((rafb_u32)(tick-s->worker_tick)>s->worker_budget) {
    s->state=RAFB_WD2_ISOLATED;s->event=RAFB_WD2_EVENT_WORKER_STALE;return 0;
  }
  return 1;
}
int rafb_wd2_rearm(rafb_wd2 *s, rafb_u32 source_stamp,
                   rafb_u32 explicit_authority, rafb_u32 tick) {
  rafb_u32 next;
  if (!checked(s) || s->state==RAFB_WD2_ACTIVE ||
      explicit_authority!=RAFB_WD2_REARM_EXPLICIT ||
      source_stamp!=s->source_stamp ||
      (rafb_u32)(tick-s->last_tick)>=0x80000000u || s->epoch==0xffffffffu) return 0;
  next=s->epoch+1u;
  s->worker_tick=tick;s->guard_tick=tick;s->last_tick=tick;
  s->worker_seq=0;s->guard_seq=0;s->last_guard_worker_seq=0;
  s->epoch=next;s->state=RAFB_WD2_ACTIVE;s->event=RAFB_WD2_EVENT_REARM;
  return 1;
}
