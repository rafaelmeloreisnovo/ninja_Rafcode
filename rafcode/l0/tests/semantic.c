/* Copyright 2026 Rafael Melo Reis */
#include "raf_l0.h"

static int state_of(const raf_l0_result* r) {
  return (int)((r->meta & RAF_L0_META_STATE_MASK) >> RAF_L0_META_STATE_SHIFT);
}

int main(void) {
  static const raf_u8 nul[1] = {0x00u};
  static const raf_u8 space[1] = {0x20u};
  static const raf_u8 byte[1] = {0x41u};
  static const raf_u8 many[2] = {0x41u, 0x42u};
  const raf_l0_view cases[] = {
    {(const raf_u8*)0, 0u, 0u},
    {(const raf_u8*)0, 0u, 1u},
    {nul, 1u, 1u},
    {space, 1u, 1u},
    {byte, 1u, 1u},
    {many, 2u, 1u},
    {(const raf_u8*)0, 1u, 1u},
  };
  const int expected[] = {
    RAF_L0_STATE_ABSENT,
    RAF_L0_STATE_EMPTY,
    RAF_L0_STATE_NUL,
    RAF_L0_STATE_SPACE,
    RAF_L0_STATE_BYTE,
    RAF_L0_STATE_SEQUENCE,
    RAF_L0_STATE_INVALID,
  };
  raf_l0_result r;
  raf_usize i;
  for (i = 0u; i < (raf_usize)(sizeof(cases) / sizeof(cases[0])); ++i) {
    raf_l0_observe(&cases[i], &r);
    if (state_of(&r) != expected[i]) return (int)(10u + i);
  }
  return 0;
}
