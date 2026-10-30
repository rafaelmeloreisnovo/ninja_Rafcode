/* Copyright 2026 Rafael Melo Reis. Outbound license: TOKEN_VAZIO. */
#ifndef RAFB_ELF_H
#define RAFB_ELF_H
#include "rafb.h"
#define RAFB_DOMAIN_ANDROID 1u
#define RAFB_DOMAIN_BARE 2u
#define RAFB_PROBE_RUNTIME 0u
#define RAFB_PROBE_OBJECT 1u
#define RAFB_PROBE_OK 1u
#define RAFB_PROBE_BLOCKED 0u
/* Caller certifies domain/host_mask using an authoritative platform gate.
 * Neither an ELF header nor the bits of this mask prove a running CPU.
 * 0xFF=NO_SELECTION; 0=first Android ARM32 ABI, never TOKEN_VAZIO. */
int rafb_probe_elf(const rafb_u8* header, rafb_u32 size,
                   rafb_u8 domain, rafb_u8 object_mode, rafb_u8* abi);
/* Candidate must be a runtime ELF ET_EXEC or ET_DYN, not ET_REL.
 * Strict: malformed/duplicate candidates abort entire choice; no heap. */
int rafb_select_elf(const rafb_u8* const* headers, const rafb_u32* sizes,
                    rafb_u8 count, rafb_u8 domain, rafb_u8 host_mask,
                    rafb_u8* selected_index, rafb_u8* selected_abi);
#endif
