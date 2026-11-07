# RAFCODE Phi — bare-metal PL011 boot V1

STATUS: SOURCE + LOCAL CROSS-COMPILE PASS_SCOPED; QEMU/HARDWARE NOT_RUN.
Scope: rafcode/x11boot/baremetal/** only. Inherited Ninja preserved.
Rights: LICENSE_STATE=TOKEN_VAZIO; OUTBOUND_LICENSE_GRANT=false.

## Modular route

    ARMv7 / AArch64 (one source per specialized module; separate ELF per ABI)
       |
       +-- entry.S       reset symbol, privileged interrupts mask, initialize SP
       +-- main.S        finite banner-byte loop, idle with WFI
       +-- uart.S        MMIO PL011 TX, at most 4096 TXFF polls per byte
       +-- link.ld       RAM entry 0x40080000 + 16 KiB NOLOAD stack
       +-- include/raf_bm_contract.h   void entry declarations, ABI-only
       +-- build.sh      compile/link/objcopy per ABI
       +-- test.sh       local structural and adversarial negative gates
       |
       +-- distinct ELF32 ARM / ELF64 AArch64 and raw binary output

No Linux syscall, Android API, C startup/runtime, C function body, libc, heap, malloc, or system library is present in the generated code. The output is not Xterm, X11, a framebuffer, Android APK or universal hardware bootloader.

## Build and test

Run from repository root:

    cd rafcode/x11boot/baremetal
    sh build.sh
    sh test.sh

Requires host Clang, LLD, llvm-objcopy, readelf, nm, and llvm-objdump as build/test instruments. No runtime linkage to them. The local build generated two executable ELFs and raw binaries. ABI/readelf/objdump gates passed for ARMv7 and AArch64. Hardware, QEMU boot and phone execution: NOT_RUN.

## Flags, warnings, layout, void and loops

- --target selects the ABI; it cannot merge different machine ISAs into one ELF.
- assembler .section, .type, .globl and linker KEEP/ASSERT establish actual build semantics.
- -nostdlib -static -fno-pie and --no-undefined reject default runtime and unresolved references.
- --gc-sections may discard unreferenced sections; it is not a guarantee of faster loops.
- -Wa,--fatal-warnings, -Wl,--fatal-warnings, -Wall, -Wextra and -Werror reject relevant diagnostics rather than rewriting instructions.
- Comments and source diagramming document proof obligations; they are not directives to the assembler.
- C header void describes entry signatures only. No C implementation is compiled into the ELF.
- The finite byte traversal delegates each byte to a separately bounded UART-poll loop; no untested loop unrolling or aliasing optimization is asserted.

## Board identity and risk boundary

QEMU virt RAM starts at 0x40000000; official QEMU documentation only guarantees flash/RAM addresses across versions, and peripherals must be discovered through DTB for portable use:
https://www.qemu.org/docs/master/system/arm/virt

The demonstration build explicitly chooses PL011 base 0x09000000 as a *profile assumption*. It is not an all-QEMU-version, all-board or Android guarantee. Physical board execution requires device-tree discovery or board-specific authorized mapping and startup firmware contract.

Pending: QEMU serial boot check, actual UART visibility, DTB validation, board reset/stack assumptions, IRQ and hardware exception handling, performance benchmarks. Do not flash this experiment to any smartphone.

SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM
claim_allowed=false for hardware runtime/Android/full Xterm.
Rollback = do not merge draft PR; no upstream files changed.

TMD_SCOPE=rafcode/x11boot/baremetal
TMD_SOURCE=original RAFCODE assembly modules
TMD_DELTA=separate bare-metal ABI and UART path
TMD_EVIDENCE=host cross-build/readelf/disassembly gates
TMD_STATE=TOKEN_VAZIO
R3=<PASS_SCOPED local build, physical boot NOT_RUN, implement DTB/QEMU serial gate>
