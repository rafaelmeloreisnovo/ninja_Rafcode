# RAFCODE build gate L0 — draft candidate, NOT Ninja replacement

Scope: ninja_Rafcode/rafcode/build/**. The upstream Ninja C++ tree and both existing authorial nuclei (command/ and l0/) are unchanged. Outbound license for new authorial code: TOKEN_VAZIO; no implicit grant.

## Architecture

The core is freestanding C with fixed capacity of 8 modules: no libc, heap, syscall, JVM, network, compiler execution or file access. Each module is a file-command descriptor with id, ABI profile, role, dependency bitset, required flags, fallback, watchdog budget and source_stamp. source_stamp is a short label, NOT a cryptographic digest.

SOURCE_FILE [HOST] -> path + cryptographic hash [HOST] -> ABI/flags -> RAFB gate -> next(id) -> compiler [HOST] -> ack(success) -> inspect artifact + receipt [HOST] -> Android packager/signing [HOST].

Failures: explicit spare only, second failure => FAILSAFE. rollback() restores the logical checkpoint only, not filesystem changes; host must provide transactional output quarantine/removal. failback() requires explicit approval and starts a new logical run. The tick source must be monotonic, with elapsed < 2^31. No execution, installation, deployment or signing occurs inside this core.

## Eight compile profiles (NOT eight device/ABI certifications)

0. Android armeabi-v7a — armv7a-linux-androideabi24
1. Android arm64-v8a — aarch64-linux-android24
2. Android x86 — i686-linux-android24
3. Android x86_64 — x86_64-linux-android24
4. bare ARMv7-M — armv7m-none-eabi
5. bare AArch64 — aarch64-none-elf
6. bare RISC-V32 — riscv32-unknown-elf
7. bare RISC-V64 — riscv64-unknown-elf

Run from repository root: sh rafcode/build/test.sh. Requires hosted cc, clang, readelf, nm. Generates unlinked ELF .o files and a native host test. ELF inspection for unresolved symbols is not proof of functional runtime, dynamic-linker absence in the final binary, APK packaging or board support. Android RafGitTools currently documents only armeabi-v7a and arm64-v8a support.

## Falsifiers and receipts

Reject: unknown ABI, missing NO_CLAIM flag, malformed bitmask, missing dependency, duplicate ID, cycle, bad spare contract, zero budget, failed spare, and unauthorized failback. 0 != NONE (0xFF) != UNKNOWN (0xFE). Changing a single bit in source, dependency or flags MUST invalidate dependent host cache entries (not yet implemented here).

The host receipt must record exact source digest, compiler+flags+target, source/dependency hashes, artifact hash, exit code, logs, TMD_SCOPE, TMD_SOURCE, TMD_DELTA, TMD_EVIDENCE, TMD_STATE, and timestamp. TMD expansion remains TOKEN_VAZIO. A recorded PASS of this L0 gate does NOT prove physical failover, filesystem transaction, APK safety or performance.

SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM
IMPLEMENTED_UNTESTED != PASS
APK=TOKEN_VAZIO | 8_ABI_RUNTIME=TOKEN_VAZIO
PHYSICAL_WATCHDOG=TOKEN_VAZIO | HOST_ROLLBACK=TOKEN_VAZIO
claim_allowed=false

Rollback of this change: close draft PR, or revert its single commit. No auto-merge and no modification to main/master.

## Plan immutability falsifier
A by-value snapshot of up to 8 module descriptors is frozen on `rafb_open`. `next`, `ack` and `watchdog` compare all descriptor fields and return FAILSAFE/event=5 on observed change; 56 one-bit negative tests (source stamp/required flags/dependency mask) are included. This is not tamper-proof against arbitrary shared-memory corruption, pointer races or adversarial source changes: a host must bind real source digests and protect memory. This is an L0 structural invariant only.
