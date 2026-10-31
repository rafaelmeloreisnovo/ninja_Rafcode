#include "rafb_wd2_bridge.h"
int rafb_wd2_gate_poll(rafb_wd2 *w, rafb_gate *g, rafb_u32 tick) {
  if (!g) return 0;
  if (!w || !rafb_wd2_guard2_poll(w, tick)) {
    /* Clear dispatch state. No implicit rollback or spare execution. */
    g->state=RAFB_FAILSAFE;
    g->active=RAFB_NONE;
    g->pending=RAFB_NONE;
    g->recover=RAFB_NONE;
    g->event_code=RAFB_EVENT_SUPERVISOR_STOP;
    return 0;
  }
  /* Invalid or terminal build state must never be called healthy. */
  if (g->state!=RAFB_READY && g->state!=RAFB_RUNNING &&
      g->state!=RAFB_COMPLETE) {
    g->state=RAFB_FAILSAFE;
    g->active=RAFB_NONE;
    g->pending=RAFB_NONE;
    g->recover=RAFB_NONE;
    g->event_code=RAFB_EVENT_SUPERVISOR_STOP;
    return 0;
  }
  return 1;
}
