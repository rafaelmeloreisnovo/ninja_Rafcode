# RAFCODE Phi — μWRITE append-only — 2026-11-07

μID: NINJA-RAFCODE-BM-20261107-01
timestamp: 2026-11-07 (America/Sao_Paulo)
source/ref: https://github.com/rafaelmeloreisnovo/ninja_Rafcode/pull/11
parent: PR #11 Linux-ELF/VT/X11 candidate, draft
kind: SOURCE_IMPL + LOCAL_TEST_RECEIPT + P0_BOUNDARY
delta: new specialist ARMv7/AArch64 bare-metal path (entry, PL011 UART, per-byte main, dedicated linker, void ABI declarations, build/tests and optional QEMU gate)
routes: P=authorial rafcode/x11boot/**; I=baremetal/README.md; E=local test.sh readelf and disassembly; R=separate Linux ELF path; A=hardware profile gate
source_limit: original RAFCODE assembly only; no copied Ninja source or license transfer
execution_target: QEMU virt experimental profile only; physical boards TOKEN_VAZIO
board_profile: PL011_MMIO=0x09000000 assumed by explicit build flag, not DTB-verified; RAM=0x40000000; entry=0x40080000
local_toolchain: Clang 17.0.0, LLD and llvm-objcopy, hosted test tools
evidence: Local ARMv7/AArch64 cross-compile PASS_SCOPED; ELF EXEC, no PT_INTERP/DT_NEEDED, undefined symbol, pending relocation, syscall/trap instruction; invalid-target and missing-board-profile gates PASS
readback: GitHub blob SHAs exactly matched compiled local sources and scripts for seven files
artifacts_local: ARMv7 ELF 5016 bytes and BIN 161 bytes; AArch64 ELF 5248 bytes and BIN 179 bytes (not installable Android binaries)
not_run: QEMU boot, physical ARM device, actual UART, real X11 GUI, Android APK
risk: hardcoded experimental UART base; no DTB parser; no board-specific reset/firmware proof; no performance benchmark
claim_allowed: false for hardware-boot, any Android installability, Xterm compatibility, benchmark gains
license_state: TOKEN_VAZIO; OUTBOUND_LICENSE_GRANT=false; upstream COPYING unchanged
TMD_SCOPE: rafcode/x11boot/baremetal
TMD_SOURCE: original RAFCODE ASM source
TMD_DELTA: segregated baremetal UART path
TMD_EVIDENCE: local host structural gates only
TMD_STATE: TOKEN_VAZIO
rollback: do not merge draft PR #11; preserve original Ninja and original Linux x11boot.S
next: run optional emulator gate with QEMU installed; replace assumed UART base with DTB lookup, then independently test real board ABI and boot chain
R3: <PASS_SCOPED local cross-build and source hash parity, NOT_RUN QEMU/physical UART, DTB discovery plus emulated boot evidence>
