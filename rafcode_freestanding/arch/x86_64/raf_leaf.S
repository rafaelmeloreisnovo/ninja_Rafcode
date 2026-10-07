/*
 * Copyright 2026 Rafael Melo Reis
 * SPDX-License-Identifier: Apache-2.0
 * Optional x86-64 leaf primitive. It is not required by raf_l0_observe.
 * Bytes: 48 89 f8 48 0f c8 c3
 */
.section .rafcode.text.asm,"ax",@progbits
.global raf_l0_bswap64_leaf
.type raf_l0_bswap64_leaf,@function
raf_l0_bswap64_leaf:
  movq %rdi, %rax
  bswap %rax
  ret
.size raf_l0_bswap64_leaf, .-raf_l0_bswap64_leaf
