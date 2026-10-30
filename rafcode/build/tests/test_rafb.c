#include "rafb.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
#define F (RAFB_NO_CLAIM|RAFB_ALLOW_FAILOVER|RAFB_ALLOW_ROLLBACK|RAFB_ALLOW_FAILBACK)
int main(void) {
  rafb_module m[3]={{0u,RAFB_ANY_ABI,RAFB_PRIMARY,1u,0u,0u,0u,5u,0x1234u},
                    {1u,RAFB_ANY_ABI,RAFB_SPARE,RAFB_NONE,0u,0u,0u,5u,0x5678u},
                    {2u,RAFB_ANY_ABI,RAFB_PRIMARY,RAFB_NONE,0u,0u,1u,5u,0xabcdU}};
  rafb_gate g; rafb_u8 id, abi, k;
  for (abi=0u;abi<8u;++abi) {
    CHECK(rafb_open(&g,m,3u,abi,F));
    CHECK(rafb_next(&g,m,1u,&id) && id==0u);
    CHECK(rafb_watchdog(&g,m,6u));
    CHECK(rafb_ack(&g,m,0u,0u)); /* failover permitted, not already success */
    CHECK(rafb_next(&g,m,7u,&id) && id==1u);
    CHECK(rafb_ack(&g,m,1u,1u));
    CHECK(rafb_next(&g,m,9u,&id) && id==2u);
    CHECK(rafb_ack(&g,m,2u,1u));
    CHECK(!rafb_next(&g,m,10u,&id) && g.state==RAFB_COMPLETE);
    CHECK(g.faults==1u && g.done==7u);
    CHECK(rafb_rollback(&g,F) && g.state==RAFB_ROLLED_BACK);
    CHECK(!rafb_failback(&g,0u));
    CHECK(rafb_failback(&g,F) && g.done==0u && g.state==RAFB_READY);
  }
  CHECK(!rafb_open(&g,m,3u,8u,F)); /* unknown ABI */
  CHECK(!rafb_open(&g,m,3u,0u,F & ~RAFB_NO_CLAIM));
  m[2].deps=1u<<7;
  CHECK(!rafb_open(&g,m,3u,0u,F)); /* no undeclared node */
  m[2].deps=1u; m[0].deps=1u<<2; m[1].deps=1u<<2;
  CHECK(!rafb_open(&g,m,3u,0u,F)); /* cycle */
  m[0].deps=0u;m[1].deps=0u;
  m[1].reserved=1u; CHECK(!rafb_open(&g,m,3u,0u,F));
  m[1].reserved=0u;
  m[1].id=0u; CHECK(!rafb_open(&g,m,3u,0u,F));
  m[1].id=1u;
  m[0].required_flags=0x0040u; CHECK(!rafb_open(&g,m,3u,0u,F));
  m[0].required_flags=0u;
  CHECK(rafb_open(&g,m,3u,0u,F));
  CHECK(rafb_next(&g,m,0u,&id) && id==0u);
  CHECK(rafb_watchdog(&g,m,5u));
  CHECK(rafb_watchdog(&g,m,6u) && g.event_code==3u); /* timeout -> failover */
  CHECK(rafb_next(&g,m,7u,&id) && id==1u);
  CHECK(!rafb_ack(&g,m,1u,0u)); /* spare failure -> FAILSAFE */
  CHECK(g.state==RAFB_FAILSAFE);
  CHECK(!rafb_next(&g,m,7u,&id));
  CHECK(rafb_rollback(&g,F));
  /* One-bit fault cascades: invalidate the dependent node, block whole graph. */
  m[2].deps ^= (1u<<7); CHECK(!rafb_open(&g,m,3u,0u,F));
  m[2].deps ^= (1u<<7); CHECK(rafb_open(&g,m,3u,0u,F));
  CHECK(!rafb_open(&g,m,3u,0u,F | 0x4000u)); /* unknown flags */
  /* Exactly 56 independent one-bit mutations after plan freeze.
   * 32 source-stamp + 16 required-flags + 8 dependency bits. */
  for (k=0u;k<56u;++k) {
    CHECK(rafb_open(&g,m,3u,0u,F));
    if (k<32u) m[0].source_stamp ^= (rafb_u32)1u<<k;
    else if (k<48u) m[0].required_flags ^= (rafb_u16)1u<<(k-32u);
    else m[2].deps ^= (rafb_u32)1u<<(k-48u);
    CHECK(!rafb_next(&g,m,1u,&id) && g.state==RAFB_FAILSAFE && g.event_code==5u);
    if (k<32u) m[0].source_stamp ^= (rafb_u32)1u<<k;
    else if (k<48u) m[0].required_flags ^= (rafb_u16)1u<<(k-32u);
    else m[2].deps ^= (rafb_u32)1u<<(k-48u);
  }
  return 0;
}
