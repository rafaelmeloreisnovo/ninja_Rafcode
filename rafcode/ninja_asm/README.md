# RAFCODE Ninja assembler-inline engine — L0 candidate

STATUS: EXPERIMENTAL / HOLD. Separate authorial implementation under `rafcode/ninja_asm/**`; the inherited Ninja C++ tree, `COPYING`, `rafcode/build`, `rafcode/command`, and `rafcode/l0` remain unchanged.

## What is implemented

- Bounded 32-node dependency DAG, deterministic lowest-ready-node selection, cycle/self/unknown dependency rejection and pinned-by-value graph mutation rejection.
- Synchronous, single-active-node scheduler with explicit READY/RUNNING/COMPLETE/FAILSAFE states, failure latching, and approved logical restart.
- GNU C11 inline ASM primitives for ARMv7 `BIC`/`RBIT`/`CLZ`, AArch64 `BIC`/`RBIT`/`CLZ`, x86-64 `NOT`/`AND`/`BSF`, with freestanding scalar fallbacks for other targets or `-DRAFN_FORCE_SCALAR=1`.
- No C++/STL/libc/malloc/heap/OS syscall in the L0 core. Compile-time compiler/toolchain is still required.

## What is NOT implemented

This is **not a complete Ninja replacement**. No `build.ninja` lexer/parser, variable expansion, command subprocesses, filesystem stat, `.ninja_log`, `.ninja_deps`, dyndep, pools, jobserver, console handling, concurrency, persistent atomic rollback, APK wiring or real Termux executable. `stamp` is an untrusted label, NOT a cryptographic source digest. Linux/Android needs a kernel and system boundary. Inline ASM is hosted inside C; fully assembly ELF is a different runtime model.

Run `sh rafcode/ninja_asm/test.sh` from the root. Local host scalar/ASM differential tests and eight compilation profiles are scoped evidence, not 8-ABI runtime validation. ARMv7 Motorola E7 physical execution, Android ABI-linked ELF, performance advantage and all security/release claims = `TOKEN_VAZIO` until tested.

## Refactoring route

1. Preserve inherited Ninja semantics and license. Compare differential behavior against a known Ninja version.
2. Finish parser/manifest subset, explicit graph compatibility semantics, edge cases, and negative tests.
3. Add isolated host adapter for filesystem, subprocess, jobserver, and cryptographic provenance; gate toolchain assumptions.
4. Integrate optionally with Termux packages consumer, validate exact Ninja build samples and Android ARMv7/ARM64 on-device; only then consider replacing the existing engine.

TMD_SCOPE=rafcode/ninja_asm/**
TMD_SOURCE=ninja_Rafcode/master@readback-required
TMD_DELTA=authorial additive L0 ASM/C11 engine
TMD_EVIDENCE=local tests and cross-compile only; provider exact-head CI TOKEN_VAZIO
TMD_STATE=TOKEN_VAZIO
LICENSE_STATE=TOKEN_VAZIO; OUTBOUND_LICENSE_GRANT=false
claim_allowed=false; merge_allowed=false; release_allowed=false
Rollback=close unmerged DRAFT PR; no upstream files overwritten.
