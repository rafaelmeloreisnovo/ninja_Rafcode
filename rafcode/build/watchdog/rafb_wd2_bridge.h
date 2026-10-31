/* Authorial L0 adapter: watchdog-of-watchdog -> build graph FAILSAFE.
 * Source contract: RAFCODE freestanding build gate v1.
 * No I/O, system API, compiler, heap, or software reset. */
#ifndef RAFB_WD2_BRIDGE_H
#define RAFB_WD2_BRIDGE_H
#include "rafb_wd2.h"
#define RAFB_EVENT_SUPERVISOR_STOP 6u
/* Must be called from an independently scheduled platform supervisor.
 * A null/unavailable supervisor is a fatal admission failure.
 * This bridge deliberately does not implement failback/rearm. */
int rafb_wd2_gate_poll(rafb_wd2 *w, rafb_gate *g, rafb_u32 tick);
#endif
