/*
 * RAFCODE Phi: one-purpose boot demonstration, no C runtime.
 * Main is a finite per-byte loop; abort on UART backpressure poll timeout.
 * Idle uses WFI, not a busy/spinning status loop.
 * This is serial output, NOT X11, framebuffer, VT parser or Android UI.
 */
#if defined(__aarch64__)
.section .rodata.banner,"a",%progbits
.balign 4
.Lbanner: .asciz "RAFCODE Phi bare metal | AArch64 PL011 | no libc\r\n"
.section .text.main,"ax",%progbits
.balign 4
.globl raf_bm_main
.type raf_bm_main,%function
raf_bm_main:
  ldr x19, =.Lbanner
1:ldrb w0, [x19], #1
  cbz w0, 3f
  bl raf_bm_uart_putc
  cbnz w0, 1b
3:wfi
  b 3b
.size raf_bm_main,.-raf_bm_main
#elif defined(__arm__)
.syntax unified
.arm
.section .rodata.banner,"a",%progbits
.balign 4
.Lbanner: .asciz "RAFCODE Phi bare metal | ARMv7 PL011 | no libc\r\n"
.section .text.main,"ax",%progbits
.balign 4
.globl raf_bm_main
.type raf_bm_main,%function
raf_bm_main:
  ldr r4, =.Lbanner
1:ldrb r0, [r4], #1
  cmp r0, #0
  beq 3f
  bl raf_bm_uart_putc
  cmp r0, #0
  bne 1b
3:wfi
  b 3b
.size raf_bm_main,.-raf_bm_main
#else
#error "bare-metal main only supports ARMv7/AArch64"
#endif
.section .note.GNU-stack,"",%progbits
