#include "rafn.h"
#define C(x) do { if (!(x)) return __LINE__; } while (0)
static rafn_u32 rng=0x9e3779b9u;
static rafn_u32 next_rand(void) {rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;}
static rafn_u8 ref_first(rafn_u32 v) {
  rafn_u8 i;
  if (!v) return RAFN_NONE;
  for (i=0u;i<32u;++i) if (v&((rafn_u32)1u<<i)) return i;
  return RAFN_NONE;
}
int main(void) {
  rafn_node n[32]; rafn_plan p; rafn_u32 i,j,k,mask; rafn_u8 id;
  C(rafn_backend()<=3u);
  C(rafn_first(0u)==RAFN_NONE);
  for (i=0u;i<4000u;++i) {
    rafn_u32 a=next_rand(),b=next_rand();
    C(rafn_unmet(a,b)==(a&~b));
    C(rafn_first(a)==ref_first(a));
  }
  C(!rafn_open(&p,n,0u)); C(p.state==RAFN_UNKNOWN);
  C(!rafn_open(&p,(const rafn_node*)0,1u));
  for(i=0u;i<32u;++i){n[i].deps=(i==0u)?0u:((rafn_u32)1u<<(i-1u));n[i].stamp=i+7u;}
  C(rafn_open(&p,n,32u));
  for (i=0u;i<32u;++i) {
    C(rafn_next(&p,n,&id) && id==i);
    C(!rafn_next(&p,n,&id)); /* cannot dispatch while running */
    C(!rafn_ack(&p,n,(rafn_u8)(i^1u),1u));
    C(rafn_ack(&p,n,(rafn_u8)i,1u));
  }
  C(!rafn_next(&p,n,&id) && p.state==RAFN_COMPLETE && id==RAFN_NONE);
  C(!rafn_restart(&p,n,0u));
  C(rafn_restart(&p,n,RAFN_RESTART_APPROVED));
  C(rafn_next(&p,n,&id) && id==0u);
  C(!rafn_ack(&p,n,id,0u) && p.state==RAFN_FAILSAFE);
  C(!rafn_next(&p,n,&id));
  C(rafn_restart(&p,n,RAFN_RESTART_APPROVED));
  n[0].stamp^=1u; C(!rafn_next(&p,n,&id) && p.state==RAFN_FAILSAFE);
  C(!rafn_restart(&p,n,RAFN_RESTART_APPROVED));n[0].stamp^=1u;
  C(rafn_restart(&p,n,RAFN_RESTART_APPROVED));
  n[0].deps=2u;C(!rafn_open(&p,n,32u));n[0].deps=0u;
  n[1].deps=(rafn_u32)1u<<2;C(!rafn_open(&p,n,32u));n[1].deps=1u;
  n[31].deps=(rafn_u32)1u<<31;C(!rafn_open(&p,n,32u));n[31].deps=(rafn_u32)1u<<30;
  C(rafn_open(&p,n,32u));
  for (k=0u;k<32u;++k) {
    n[k].stamp^=(rafn_u32)1u<<(k%32u);
    C(!rafn_next(&p,n,&id) && p.state==RAFN_FAILSAFE);
    n[k].stamp^=(rafn_u32)1u<<(k%32u);
    C(rafn_restart(&p,n,RAFN_RESTART_APPROVED));
  }
  for (j=0u;j<200u;++j) {
    /* Acyclic random DAG: dependencies only on earlier IDs. */
    for (i=0u;i<32u;++i) {
      mask=(i==0u)?0u:(((rafn_u32)1u<<i)-1u);
      n[i].deps=next_rand()&mask;n[i].stamp=next_rand();
    }
    C(rafn_open(&p,n,32u));
    for (i=0u;i<32u;++i) {
      C(rafn_next(&p,n,&id));
      C(!rafn_unmet(n[id].deps,p.done));
      C(!(p.done & ((rafn_u32)1u<<id)));
      C(rafn_ack(&p,n,id,1u));
    }
    C(!rafn_next(&p,n,&id) && p.state==RAFN_COMPLETE);
  }
  return 0;
}
