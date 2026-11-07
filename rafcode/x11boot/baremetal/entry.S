/*
 * RAFCODE Phi: ARM reset ABI only, no libc, no syscall, no heap.
 * ABI-sensitive .S entry: _start is the sole externally required boot symbol.
 * .bss/stack address supplied by the dedicated linker script.
 * Contract: one core, privileged state, RAM already initialized by board loader.
 */
#if defined(__aarch64__)
.section .text.boot,"ax",%progbits
.balign 16
.globl _start
.type _start,%function
_start:
  msr daifset, #0xf
  ldr x9, =__raf_stack_top
  mov sp, x9
  bl raf_bm_main
1: wfi
   b 1b
.size _start,.-_start
#elif defined(__arm__)
.syntax unified
.arm
.section .text.boot,"ax",%progbits
.balign 4
.globl _start
.type _start,%function
_start:
  cpsid if
  ldr sp, =__raf_stack_top
  bl raf_bm_main
1: wfi
   b 1b
.size _start,.-_start
#else
#error "bare-metal boot only supports ARMv7/AArch64"
#endif
.section .note.GNU-stack,"",%progbits
