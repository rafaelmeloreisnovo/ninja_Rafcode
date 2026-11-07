/*
 * RAFCODE Phi: board-specialist PL011 TX only (bare-metal MMIO).
 * REQUIRED: RAF_UART_BASE supplied explicitly by build profile.
 * ARM EABI: input r0=byte, output r0=1 if sent else 0 (timeout).
 * AArch64: input w0=byte, output w0=1 if sent else 0 (timeout).
 * Clobber only scratch registers; no state, globals, heap or syscalls.
 * FR @ 0x18, TXFF bit 5. Poll budget 4096 reads, not a time guarantee.
 */
#ifndef RAF_UART_BASE
#error "RAF_UART_BASE required: board profile must define its mapped address"
#endif
#if defined(__aarch64__)
.section .text.uart,"ax",%progbits
.balign 4
.globl raf_bm_uart_putc
.type raf_bm_uart_putc,%function
raf_bm_uart_putc:
  ldr x1, =RAF_UART_BASE
  mov w3, #4096
1:ldr w2, [x1, #0x18]
  tbz w2, #5, 2f
  subs w3, w3, #1
  b.ne 1b
  mov w0, #0
  ret
2:str w0, [x1]
  mov w0, #1
  ret
.size raf_bm_uart_putc,.-raf_bm_uart_putc
#elif defined(__arm__)
.syntax unified
.arm
.section .text.uart,"ax",%progbits
.balign 4
.globl raf_bm_uart_putc
.type raf_bm_uart_putc,%function
raf_bm_uart_putc:
  ldr r1, =RAF_UART_BASE
  mov r3, #4096
1:ldr r2, [r1, #0x18]
  tst r2, #0x20
  beq 2f
  subs r3, r3, #1
  bne 1b
  mov r0, #0
  bx lr
2:str r0, [r1]
  mov r0, #1
  bx lr
.size raf_bm_uart_putc,.-raf_bm_uart_putc
#else
#error "PL011 UART bare-metal module only supports ARMv7/AArch64"
#endif
.section .note.GNU-stack,"",%progbits
