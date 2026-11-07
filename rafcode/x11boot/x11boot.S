/*
 * RAFCODE Phi — freestanding ELF + VT stream + X11 setup probe.
 * Authorial boundary: rafcode/x11boot. No borrowed Ninja implementation.
 * No C variables/functions, libc, heap, TLS, dynamic linker or runtime framework.
 * HOSTED LINUX SYSCALLS ARE REQUIRED. X11 rendering is NOT implemented.
 * License outbound: TOKEN_VAZIO until owner authorizes it.
 * Source is one .S for x86_64, AArch64, ARMv7 EABI. One ELF per ABI.
 */
#if !defined(__x86_64__) && !defined(__aarch64__) && !defined(__arm__)
#error "Unsupported ELF ABI: explicit port and test required"
#endif
#ifndef RAF_X11_SOCKET
#define RAF_X11_SOCKET "/tmp/.X11-unix/X0"
#endif

.section .rodata
.p2align 2
1: .ascii "\033[0mRAFCODE Phi | ELF freestanding | VT stream\r\n"
2:
3: .ascii "X11_FRAME_OK (full bounded setup reply, no window)\r\n"
4:
5: .ascii "X11_UNAVAILABLE (auth, socket or setup; VT fallback active)\r\n"
6:
.p2align 2
7: .short 1
   .asciz RAF_X11_SOCKET
8:
.if (8b-7b) != 20
.error "X11 socket path must have 17 bytes in V1"
.endif
.p2align 2
9: .byte 0x6c, 0x00
   .short 11, 0
   .short 0, 0, 0
10:

.section .text
.globl _start
.type _start, %function
_start:
#if defined(__x86_64__)
  sub $2064, %rsp
  mov $1, %eax
  mov $1, %edi
  lea 1b(%rip), %rsi
  mov $48, %edx
  syscall

  mov $41, %eax
  mov $1, %edi
  mov $1, %esi
  xor %edx, %edx
  syscall
  test %rax, %rax
  js .Lx11_fail_x64
  mov %rax, %r12

  mov $42, %eax
  mov %r12, %rdi
  lea 7b(%rip), %rsi
  mov $20, %edx
  syscall
  test %rax, %rax
  js .Lx11_close_fail_x64

  mov $1, %eax
  mov %r12, %rdi
  lea 9b(%rip), %rsi
  mov $12, %edx
  syscall
  cmp $12, %rax
  jne .Lx11_close_fail_x64

  xor %r13, %r13
.Lx11_read_x64:
  xor %eax, %eax
  mov %r12, %rdi
  lea (%rsp,%r13), %rsi
  mov $8, %rdx
  sub %r13, %rdx
  syscall
  test %rax, %rax
  jle .Lx11_close_fail_x64
  add %rax, %r13
  cmp $8, %r13
  jne .Lx11_read_x64
  cmpb $1, (%rsp)
  jne .Lx11_close_fail_x64
  cmpb $11, 2(%rsp)
  jne .Lx11_close_fail_x64
  cmpb $0, 3(%rsp)
  jne .Lx11_close_fail_x64
  movzwl 6(%rsp), %r14d
  shl $2, %r14d
  cmp $32, %r14d
  jb .Lx11_close_fail_x64
  cmp $2048, %r14d
  ja .Lx11_close_fail_x64
  xor %r15d, %r15d
.Lx11_body_x64:
  xor %eax, %eax
  mov %r12, %rdi
  lea (%rsp,%r15), %rsi
  mov %r14, %rdx
  sub %r15, %rdx
  syscall
  test %rax, %rax
  jle .Lx11_close_fail_x64
  add %rax, %r15
  cmp %r14, %r15
  jne .Lx11_body_x64
  mov $3, %eax
  mov %r12, %rdi
  syscall
  mov $1, %eax
  mov $1, %edi
  lea 3b(%rip), %rsi
  mov $52, %edx
  syscall
  jmp .Lvt_x64
.Lx11_close_fail_x64:
  mov $3, %eax
  mov %r12, %rdi
  syscall
.Lx11_fail_x64:
  mov $1, %eax
  mov $1, %edi
  lea 5b(%rip), %rsi
  mov $61, %edx
  syscall
.Lvt_x64:
  xor %eax, %eax
  xor %edi, %edi
  mov %rsp, %rsi
  mov $2048, %edx
  syscall
  test %rax, %rax
  jle .Lexit_x64
  mov %rax, %r13
  xor %r14d, %r14d
.Lvt_write_x64:
  mov $1, %eax
  mov $1, %edi
  lea (%rsp,%r14), %rsi
  mov %r13, %rdx
  sub %r14, %rdx
  syscall
  test %rax, %rax
  jle .Lexit_x64
  add %rax, %r14
  cmp %r13, %r14
  jne .Lvt_write_x64
  jmp .Lvt_x64
.Lexit_x64:
  xor %edi, %edi
  mov $60, %eax
  syscall
  ud2

#elif defined(__aarch64__)
  sub sp, sp, #2064
  mov x0, #1
  adrp x1, 1b
  add x1, x1, :lo12:1b
  mov x2, #48
  mov x8, #64
  svc #0

  mov x0, #1
  mov x1, #1
  mov x2, #0
  mov x8, #198
  svc #0
  cmp x0, #0
  b.lt .Lx11_fail_a64
  mov x19, x0

  mov x0, x19
  adrp x1, 7b
  add x1, x1, :lo12:7b
  mov x2, #20
  mov x8, #203
  svc #0
  cmp x0, #0
  b.lt .Lx11_close_fail_a64

  mov x0, x19
  adrp x1, 9b
  add x1, x1, :lo12:9b
  mov x2, #12
  mov x8, #64
  svc #0
  cmp x0, #12
  b.ne .Lx11_close_fail_a64

  mov x20, #0
.Lx11_read_a64:
  mov x0, x19
  add x1, sp, x20
  mov x2, #8
  sub x2, x2, x20
  mov x8, #63
  svc #0
  cmp x0, #0
  b.le .Lx11_close_fail_a64
  add x20, x20, x0
  cmp x20, #8
  b.ne .Lx11_read_a64
  ldrb w9, [sp]
  cmp w9, #1
  b.ne .Lx11_close_fail_a64
  ldrb w9, [sp, #2]
  cmp w9, #11
  b.ne .Lx11_close_fail_a64
  ldrb w9, [sp, #3]
  cbnz w9, .Lx11_close_fail_a64
  ldrh w21, [sp, #6]
  lsl x21, x21, #2
  cmp x21, #32
  b.lo .Lx11_close_fail_a64
  cmp x21, #2048
  b.hi .Lx11_close_fail_a64
  mov x22, #0
.Lx11_body_a64:
  mov x0, x19
  add x1, sp, x22
  sub x2, x21, x22
  mov x8, #63
  svc #0
  cmp x0, #0
  b.le .Lx11_close_fail_a64
  add x22, x22, x0
  cmp x22, x21
  b.ne .Lx11_body_a64
  mov x0, x19
  mov x8, #57
  svc #0
  mov x0, #1
  adrp x1, 3b
  add x1, x1, :lo12:3b
  mov x2, #52
  mov x8, #64
  svc #0
  b .Lvt_a64
.Lx11_close_fail_a64:
  mov x0, x19
  mov x8, #57
  svc #0
.Lx11_fail_a64:
  mov x0, #1
  adrp x1, 5b
  add x1, x1, :lo12:5b
  mov x2, #61
  mov x8, #64
  svc #0
.Lvt_a64:
  mov x0, #0
  mov x1, sp
  mov x2, #2048
  mov x8, #63
  svc #0
  cmp x0, #0
  b.le .Lexit_a64
  mov x20, x0
  mov x21, #0
.Lvt_write_a64:
  mov x0, #1
  add x1, sp, x21
  sub x2, x20, x21
  mov x8, #64
  svc #0
  cmp x0, #0
  b.le .Lexit_a64
  add x21, x21, x0
  cmp x21, x20
  b.ne .Lvt_write_a64
  b .Lvt_a64
.Lexit_a64:
  mov x0, #0
  mov x8, #93
  svc #0
  brk #0

#elif defined(__arm__)
  .arm
  sub sp, sp, #2048
  sub sp, sp, #16
  mov r0, #1
  ldr r1, =1b
  mov r2, #48
  mov r7, #4
  svc #0

  mov r0, #1
  mov r1, #1
  mov r2, #0
  mov r7, #281
  svc #0
  cmp r0, #0
  blt .Lx11_fail_arm
  mov r4, r0

  mov r0, r4
  ldr r1, =7b
  mov r2, #20
  mov r7, #283
  svc #0
  cmp r0, #0
  blt .Lx11_close_fail_arm

  mov r0, r4
  ldr r1, =9b
  mov r2, #12
  mov r7, #4
  svc #0
  cmp r0, #12
  bne .Lx11_close_fail_arm

  mov r5, #0
.Lx11_read_arm:
  mov r0, r4
  add r1, sp, r5
  rsb r2, r5, #8
  mov r7, #3
  svc #0
  cmp r0, #0
  ble .Lx11_close_fail_arm
  add r5, r5, r0
  cmp r5, #8
  bne .Lx11_read_arm
  ldrb r6, [sp]
  cmp r6, #1
  bne .Lx11_close_fail_arm
  ldrb r6, [sp, #2]
  cmp r6, #11
  bne .Lx11_close_fail_arm
  ldrb r6, [sp, #3]
  cmp r6, #0
  bne .Lx11_close_fail_arm
  ldrb r8, [sp, #6]
  ldrb r9, [sp, #7]
  orr r8, r8, r9, lsl #8
  lsl r8, r8, #2
  cmp r8, #32
  blo .Lx11_close_fail_arm
  cmp r8, #2048
  bhi .Lx11_close_fail_arm
  mov r9, #0
.Lx11_body_arm:
  mov r0, r4
  add r1, sp, r9
  sub r2, r8, r9
  mov r7, #3
  svc #0
  cmp r0, #0
  ble .Lx11_close_fail_arm
  add r9, r9, r0
  cmp r9, r8
  bne .Lx11_body_arm
  mov r0, r4
  mov r7, #6
  svc #0
  mov r0, #1
  ldr r1, =3b
  mov r2, #52
  mov r7, #4
  svc #0
  b .Lvt_arm
.Lx11_close_fail_arm:
  mov r0, r4
  mov r7, #6
  svc #0
.Lx11_fail_arm:
  mov r0, #1
  ldr r1, =5b
  mov r2, #61
  mov r7, #4
  svc #0
.Lvt_arm:
  mov r0, #0
  mov r1, sp
  mov r2, #2048
  mov r7, #3
  svc #0
  cmp r0, #0
  ble .Lexit_arm
  mov r5, r0
  mov r6, #0
.Lvt_write_arm:
  mov r0, #1
  add r1, sp, r6
  sub r2, r5, r6
  mov r7, #4
  svc #0
  cmp r0, #0
  ble .Lexit_arm
  add r6, r6, r0
  cmp r6, r5
  bne .Lvt_write_arm
  b .Lvt_arm
.Lexit_arm:
  mov r0, #0
  mov r7, #1
  svc #0
  udf #0
  .ltorg
#endif
.size _start, .-_start
.section .note.GNU-stack,""
