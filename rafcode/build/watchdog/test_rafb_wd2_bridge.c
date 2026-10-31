#include "rafb_wd2_bridge.h"
#define T(x) do {if(!(x))return __LINE__;}while(0)
int main(void) {
  rafb_wd2 s;
  rafb_gate g={0};
  g.active=1u;g.pending=2u;g.recover=3u;g.state=RAFB_RUNNING;
  T(rafb_wd2_open(&s,0xabcdu,10u,5u,0u));
  T(rafb_wd2_worker_progress(&s,1u,1u));
  T(rafb_wd2_guard1_receipt(&s,1u,1u,2u));
  T(rafb_wd2_gate_poll(&s,&g,3u));
  T(!rafb_wd2_gate_poll(&s,&g,9u)); /* Guard1 dead, worker also overdue */
  T(g.state==RAFB_FAILSAFE && g.active==RAFB_NONE &&
    g.pending==RAFB_NONE && g.recover==RAFB_NONE &&
    g.event_code==RAFB_EVENT_SUPERVISOR_STOP);
  T(!rafb_wd2_gate_poll(&s,&g,10u));
  T(rafb_wd2_rearm(&s,s.source_stamp,RAFB_WD2_REARM_EXPLICIT,10u));
  T(!rafb_wd2_gate_poll(&s,&g,11u)); /* Re-armed monitor MUST NOT auto-restart gate */
  T(g.state==RAFB_FAILSAFE);
  g.state=RAFB_READY;g.active=RAFB_NONE;
  T(rafb_wd2_worker_progress(&s,1u,11u));
  T(rafb_wd2_guard1_receipt(&s,1u,1u,12u));
  T(rafb_wd2_gate_poll(&s,&g,13u));
  T(!rafb_wd2_gate_poll((rafb_wd2*)0,&g,14u));
  T(g.state==RAFB_FAILSAFE);
  T(!rafb_wd2_gate_poll(&s,(rafb_gate*)0,15u));
  T(!rafb_wd2_open(&s,0u,1u,1u,16u));
  T(s.state==RAFB_WD2_HALTED && s.event==RAFB_WD2_EVENT_TAMPER);
  return 0;
}
