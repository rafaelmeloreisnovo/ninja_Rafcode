/* Copyright 2026 Rafael Melo Reis
 */
/* Optional authored leaf: pure register permutation, not required by the core. */
.section .rafcode.text.arch,"ax",@progbits
.global rafk_rotl13_leaf
.type rafk_rotl13_leaf,@function
rafk_rotl13_leaf:
  movq %rdi, %rax
  rolq $13, %rax
  ret
.size rafk_rotl13_leaf, .-rafk_rotl13_leaf
