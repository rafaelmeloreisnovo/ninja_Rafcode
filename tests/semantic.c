/* Copyright 2026 Rafael Melo Reis
 * SPDX-License-Identifier: Apache-2.0
 */
#include "rafk_protocol.h"

static int check(raf_u8 got, raf_u8 want, int code) {
  return got == want ? 0 : code;
}

int main(void) {
  raf_state s = {0};
  raf_event e = {0};
  raf_frame f;
  int r;

  f = (raf_frame){RAF_OP_BOOT, RAF_FLAG_PRESENT, 3u, 1u, 2u, 3u};
  rafk_step(&f, &s, &e);
  r = check(e.status, RAF_STATUS_OK, 10); if (r) return r;

  f = (raf_frame){RAF_OP_DECLARE, RAF_FLAG_PRESENT, 3u, 77u, 5u, 9u};
  rafk_step(&f, &s, &e);
  r = check(e.status, RAF_STATUS_OK, 11); if (r) return r;

  f = (raf_frame){RAF_OP_ARM, RAF_FLAG_PRESENT | RAF_FLAG_AUTHORIZED, 3u, 77u, 0u, 0u};
  rafk_step(&f, &s, &e);
  r = check(e.status, RAF_STATUS_OK, 12); if (r) return r;

  f = (raf_frame){RAF_OP_COMMIT, RAF_FLAG_PRESENT, 3u, 77u, 11u, 12u};
  rafk_step(&f, &s, &e);
  r = check(e.status, RAF_STATUS_OK, 13); if (r) return r;

  f = (raf_frame){RAF_OP_HALT, RAF_FLAG_PRESENT, 3u, 0u, 0u, 0u};
  rafk_step(&f, &s, &e);
  r = check(e.status, RAF_STATUS_HALTED, 14); if (r) return r;

  f = (raf_frame){RAF_OP_RESET, RAF_FLAG_PRESENT, 3u, 0u, 0u, 0u};
  rafk_step(&f, &s, &e);
  r = check(e.status, RAF_STATUS_HALTED, 15); if (r) return r;

  return (rafk_profile() == RAFK_PROFILE_V1) ? 0 : 16;
}
