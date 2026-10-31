#include "rafb_sha3.h"
#define TEST(x) do{if(!(x))return __LINE__;}while(0)
static rafb_u8 nib(char ch) {
  if(ch>='0'&&ch<='9')return (rafb_u8)(ch-'0');
  if(ch>='a'&&ch<='f')return (rafb_u8)(10+ch-'a');
  return 0xffu;
}
static void decode(const char hex[65], rafb_u8 out[32]) {
  rafb_u32 j;
  for(j=0u;j<32u;++j)out[j]=(rafb_u8)((nib(hex[j*2u])<<4u)|nib(hex[j*2u+1u]));
}
static const char *expected[6]={
 "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a",
 "3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532",
 "fded8fd9d6551c601eeb3b7c6bc5e5cfd8aad1d015b7e9aaa9c9b9475231d5e2",
 "cf3ccff92480a29160c2d38317c430e14749bfee1788106957dfe73f8c4930e5",
 "ce9d7dc90913ee5d92745019479a5352c6d6279bef18ed07dc0a83ee8084daca",
 "48e66a01861d0eadaacdb7a6ae7db6b9ac79242ecced4154a9fbb33c4e3cc571"
};
static const rafb_u32 sizes[6]={0u,3u,135u,136u,137u,1000u};
int main(void){
  rafb_u8 data[1000],digest[32],ref[32],fragmented[32];
  rafb_sha3 s;
  rafb_u32 i,j,k;
  for(j=0u;j<1000u;++j)data[j]=(rafb_u8)(j%251u);
  for(i=0u;i<6u;++i){
    rafb_u32 n=sizes[i];
    decode(expected[i],ref);
    if(i==1u){data[0]='a';data[1]='b';data[2]='c';}
    rafb_sha3_init(&s);
    TEST(rafb_sha3_update(&s, data, n));
    TEST(rafb_sha3_final(&s,digest));
    TEST(rafb_sha3_equal(digest,ref));
    TEST(!rafb_sha3_update(&s,data,1u));
    TEST(!rafb_sha3_final(&s,digest));
    rafb_sha3_init(&s);
    for(k=0u;k<n;++k) TEST(rafb_sha3_update(&s,data+k,1u));
    TEST(rafb_sha3_final(&s,fragmented));
    TEST(rafb_sha3_equal(fragmented,ref));
    if(i==1u){data[0]=0u;data[1]=1u;data[2]=2u;}
  }
  rafb_sha3_init(&s);
  TEST(!rafb_sha3_update(&s,(const rafb_u8*)0,1u));
  TEST(s.status==RAFB_SHA3_ERROR);
  rafb_sha3_init(&s);
  TEST(rafb_sha3_update(&s,(const rafb_u8*)0,0u));
  TEST(rafb_sha3_final(&s,digest));
  decode(expected[0],ref);
  TEST(rafb_sha3_equal(digest,ref));
  TEST(!rafb_sha3_equal(digest,(const rafb_u8*)0));
  /* Single-bit negative probes: 56 one-bit changes in this fixture. */
  for(i=0u;i<56u;++i){
    data[i/8u]^=(rafb_u8)(1u<<(i%8u));
    rafb_sha3_init(&s);
    TEST(rafb_sha3_update(&s,data,137u));
    TEST(rafb_sha3_final(&s,digest));
    decode(expected[4],ref);
    TEST(!rafb_sha3_equal(digest,ref));
    data[i/8u]^=(rafb_u8)(1u<<(i%8u));
  }
  return 0;
}
