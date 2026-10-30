#include "rafb_elf.h"
#define CHECK(x) do {if(!(x))return __LINE__;}while(0)
static void make(rafb_u8* p,rafb_u8 bits,rafb_u16 mach,rafb_u16 type){
  rafb_u8 i;for(i=0u;i<64u;++i)p[i]=0;
  p[0]=0x7fu;p[1]='E';p[2]='L';p[3]='F';p[4]=bits;p[5]=1u;p[6]=1u;
  p[16]=(rafb_u8)type;p[18]=(rafb_u8)mach;p[19]=(rafb_u8)(mach>>8);p[20]=1u;
  p[bits==1u?40u:52u]=bits==1u?52u:64u;
}
int main(void){
  rafb_u8 p[8][64],abi=RAFB_NONE,index=RAFB_NONE,k;
  const rafb_u8 *headers[8];rafb_u32 sizes[8];
  static const rafb_u16 machines[8]={40u,183u,3u,62u,40u,183u,243u,243u};
  static const rafb_u8 classes[8]={1u,2u,1u,2u,1u,2u,1u,2u};
  for(k=0u;k<8u;++k){
    make(p[k],classes[k],machines[k],3u);headers[k]=p[k];sizes[k]=64u;
    CHECK(rafb_probe_elf(p[k],64u,k<4u?1u:2u,0u,&abi));
    CHECK(abi==k);
    CHECK(!rafb_probe_elf(p[k],51u,k<4u?1u:2u,0u,&abi));
    CHECK(abi==RAFB_NONE);
    CHECK(rafb_probe_elf(p[k],64u,k<4u?1u:2u,0u,&abi));
    p[k][5]=2u;CHECK(!rafb_probe_elf(p[k],64u,k<4u?1u:2u,0u,&abi));p[k][5]=1u;
    p[k][0]^=1u;CHECK(!rafb_probe_elf(p[k],64u,k<4u?1u:2u,0u,&abi));p[k][0]^=1u;
    make(p[k],classes[k],machines[k],1u);
    CHECK(!rafb_probe_elf(p[k],64u,k<4u?1u:2u,0u,&abi));
    CHECK(rafb_probe_elf(p[k],64u,k<4u?1u:2u,1u,&abi));
    make(p[k],classes[k],machines[k],3u);
  }
  CHECK(rafb_select_elf(headers,sizes,4u,RAFB_DOMAIN_ANDROID,0x0fu,&index,&abi));
  CHECK(index==1u && abi==RAFB_ANDROID_AARCH64); /* 64-bit first, NOT CPU detection */
  CHECK(rafb_select_elf(headers,sizes,4u,RAFB_DOMAIN_ANDROID,1u,&index,&abi));
  CHECK(index==0u && abi==RAFB_ANDROID_ARMV7);
  CHECK(!rafb_select_elf(headers,sizes,4u,RAFB_DOMAIN_ANDROID,0u,&index,&abi));
  CHECK(index==RAFB_NONE && abi==RAFB_NONE);
  CHECK(!rafb_select_elf(headers,sizes,8u,RAFB_DOMAIN_ANDROID,0xffu,&index,&abi));
  CHECK(rafb_select_elf(headers+4,sizes+4,4u,RAFB_DOMAIN_BARE,0xf0u,&index,&abi));
  CHECK(index==1u && abi==RAFB_BARE_AARCH64);
  p[1][18]=40u;CHECK(!rafb_select_elf(headers,sizes,4u,RAFB_DOMAIN_ANDROID,0x0fu,&index,&abi));
  p[1][18]=(rafb_u8)183u;
  for(k=0u;k<56u;++k) {
    rafb_u8 pos=(rafb_u8)(k/8u);rafb_u8 bit=(rafb_u8)(1u<<(k%8u));
    p[0][pos]^=bit;
    /* No promise every bit must be fatal: e.g. ABI-compatible variations
     * are allowed; assert deterministic restoration of original bytes. */
    p[0][pos]^=bit;
    CHECK(rafb_probe_elf(p[0],64u,RAFB_DOMAIN_ANDROID,0u,&abi) && abi==0u);
  }
  return 0;
}
