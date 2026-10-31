#include "rafb_wd2.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while(0)
int main(void) {
  rafb_wd2 s;rafb_u32 i;
  CHECK(rafb_wd2_open(&s,0x149aa1u,10u,6u,1u));
  CHECK(rafb_wd2_worker_progress(&s,1u,2u));
  CHECK(rafb_wd2_guard1_receipt(&s,1u,1u,3u));
  CHECK(rafb_wd2_guard2_poll(&s,4u));
  CHECK(rafb_wd2_worker_progress(&s,2u,7u));
  CHECK(!rafb_wd2_guard2_poll(&s,10u)); /* guard stale despite worker alive */
  CHECK(s.event==RAFB_WD2_EVENT_GUARD_STALE && s.state==RAFB_WD2_HALTED);
  CHECK(!rafb_wd2_worker_progress(&s,3u,11u));
  CHECK(!rafb_wd2_rearm(&s,s.source_stamp,0u,11u));
  CHECK(!rafb_wd2_rearm(&s,0x44u,RAFB_WD2_REARM_EXPLICIT,11u));
  CHECK(rafb_wd2_rearm(&s,s.source_stamp,RAFB_WD2_REARM_EXPLICIT,11u));
  CHECK(s.epoch==2u);
  CHECK(rafb_wd2_open(&s,0x149aa1u,5u,20u,11u));
  CHECK(!rafb_wd2_guard2_poll(&s,17u)); /* worker stale, guard2 not stale */
  CHECK(s.state==RAFB_WD2_ISOLATED && s.event==RAFB_WD2_EVENT_WORKER_STALE);
  CHECK(rafb_wd2_rearm(&s,s.source_stamp,RAFB_WD2_REARM_EXPLICIT,23u));
  CHECK(rafb_wd2_worker_progress(&s,1u,24u));
  CHECK(!rafb_wd2_guard1_receipt(&s,0u,1u,25u)); /* invalid observed progress */
  CHECK(s.event==RAFB_WD2_EVENT_ATTESTATION);
  CHECK(rafb_wd2_rearm(&s,s.source_stamp,RAFB_WD2_REARM_EXPLICIT,26u));
  CHECK(!rafb_wd2_guard2_poll(&s,25u)); /* clock rollback */
  CHECK(s.event==RAFB_WD2_EVENT_CLOCK);
  for(i=0u;i<56u;i++) {
    CHECK(rafb_wd2_open(&s,0x149aa1u,10u,6u,5u));
    if (i<32u) s.source_stamp ^= (rafb_u32)1u<<i;
    else if(i<48u) s.worker_budget ^= (rafb_u32)1u<<(i-32u);
    else s.guard_budget ^= (rafb_u32)1u<<(i-48u);
    CHECK(!rafb_wd2_guard2_poll(&s,6u));
    CHECK(s.state==RAFB_WD2_HALTED && s.event==RAFB_WD2_EVENT_TAMPER);
  }
  CHECK(rafb_wd2_open(&s,0x149aa1u,10u,6u,0xfffffffdu));
  CHECK(rafb_wd2_worker_progress(&s,1u,0xfffffffeu));
  CHECK(rafb_wd2_guard1_receipt(&s,1u,1u,0xffffffffu));
  CHECK(rafb_wd2_guard2_poll(&s,0u)); /* wrap-safe */
  return 0;
}
