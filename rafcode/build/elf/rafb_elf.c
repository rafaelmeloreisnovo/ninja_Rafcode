/* ELF byte verifier, pure freestanding C. No OS/NDK/SDK/JNI/ART calls. */
#include "rafb_elf.h"
static rafb_u16 le16(const rafb_u8* b) {
  return (rafb_u16)((rafb_u16)b[0] | (rafb_u16)((rafb_u16)b[1]<<8));
}
static rafb_u32 le32(const rafb_u8* b) {
  return (rafb_u32)b[0] | ((rafb_u32)b[1]<<8) |
         ((rafb_u32)b[2]<<16) | ((rafb_u32)b[3]<<24);
}
int rafb_probe_elf(const rafb_u8* p, rafb_u32 n,
                   rafb_u8 domain, rafb_u8 object_mode, rafb_u8* out) {
  rafb_u8 cl, a=RAFB_NONE;
  rafb_u16 type, machine;
  if (out) *out=RAFB_NONE;
  if (!p || !out || (domain!=RAFB_DOMAIN_ANDROID && domain!=RAFB_DOMAIN_BARE) ||
      object_mode>RAFB_PROBE_OBJECT || n<52u) return RAFB_PROBE_BLOCKED;
  if (p[0]!=0x7fu || p[1]!='E' || p[2]!='L' || p[3]!='F' ||
      (p[4]!=1u && p[4]!=2u) || p[5]!=1u || p[6]!=1u ||
      (p[7]!=0u && p[7]!=3u) || p[8]!=0u) return RAFB_PROBE_BLOCKED;
  cl=p[4];
  if (cl==2u && n<64u) return RAFB_PROBE_BLOCKED;
  type=le16(p+16u); machine=le16(p+18u);
  if (le32(p+20u)!=1u ||
      (type!=2u && type!=3u && !(object_mode && type==1u)) ||
      le16(p+(cl==1u?40u:52u))!=(cl==1u?52u:64u)) return RAFB_PROBE_BLOCKED;
  if (machine==40u && cl==1u) a=(domain==RAFB_DOMAIN_ANDROID)?RAFB_ANDROID_ARMV7:RAFB_BARE_ARMV7M;
  if (machine==183u && cl==2u) a=(domain==RAFB_DOMAIN_ANDROID)?RAFB_ANDROID_AARCH64:RAFB_BARE_AARCH64;
  if (machine==3u && cl==1u && domain==RAFB_DOMAIN_ANDROID) a=RAFB_ANDROID_X86;
  if (machine==62u && cl==2u && domain==RAFB_DOMAIN_ANDROID) a=RAFB_ANDROID_X86_64;
  if (machine==243u && cl==1u && domain==RAFB_DOMAIN_BARE) a=RAFB_BARE_RV32;
  if (machine==243u && cl==2u && domain==RAFB_DOMAIN_BARE) a=RAFB_BARE_RV64;
  if (a==RAFB_NONE) return RAFB_PROBE_BLOCKED;
  *out=a;return RAFB_PROBE_OK;
}
int rafb_select_elf(const rafb_u8* const* headers, const rafb_u32* sizes,
                    rafb_u8 count, rafb_u8 domain, rafb_u8 allowed,
                    rafb_u8* index, rafb_u8* abi) {
  /* Architectural preference is explicit; allowed mask is external proof. */
  static const rafb_u8 priority[8]={1u,0u,3u,2u,5u,4u,7u,6u};
  rafb_u8 i,j,a,found[8],indices[8];rafb_u8 seen=0u;
  if (index) *index=RAFB_NONE;
  if (abi) *abi=RAFB_NONE;
  if (!headers || !sizes || !index || !abi || count==0u || count>8u ||
      allowed==0u || (domain!=RAFB_DOMAIN_ANDROID && domain!=RAFB_DOMAIN_BARE)) return 0;
  /* Every supplied image must be well formed and unique. */
  for (i=0u;i<count;++i) {
    if (!rafb_probe_elf(headers[i],sizes[i],domain,RAFB_PROBE_RUNTIME,&a)) return 0;
    if (seen & ((rafb_u8)1u<<a)) return 0;
    seen=(rafb_u8)(seen | ((rafb_u8)1u<<a));
    found[i]=a;indices[i]=i;
  }
  for (j=0u;j<8u;++j) {
    a=priority[j];
    if (!(allowed & ((rafb_u8)1u<<a))) continue;
    for (i=0u;i<count;++i) if (found[i]==a) {
      *abi=a;*index=indices[i];return 1;
    }
  }
  return 0;
}
