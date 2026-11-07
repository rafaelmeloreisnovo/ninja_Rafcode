/*
 * RAFCODE Phi: optional ABI-only interface, no native C implementation.
 * Only declarations: no variables, objects, allocations or runtime calls.
 * void denotes no C return value, NOT a hardware instruction.
 */
#ifndef RAF_BM_CONTRACT_H
#define RAF_BM_CONTRACT_H
#if !defined(__arm__) && !defined(__aarch64__)
#error "RAFCODE baremetal ABI supports only ARMv7/AArch64"
#endif
#if defined(__STDC_HOSTED__) && __STDC_HOSTED__ != 0
#error "Host C mode is forbidden for this baremetal contract"
#endif
#ifdef __cplusplus
#error "C++ ABI is not supported"
#endif
extern void _start(void) __attribute__((noreturn));
extern void raf_bm_main(void) __attribute__((noreturn));
/* Register r0/w0 contains byte on entry; returns 1 on successful TX or 0. */
extern unsigned int raf_bm_uart_putc(unsigned int);
#endif
