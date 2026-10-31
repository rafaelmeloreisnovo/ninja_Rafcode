# RAFCODE · SHA3-256 freestanding

This is independently authored C11 implementation of the public SHA3-256 algorithm specified by **NIST FIPS 202**. No libc, heap, syscalls, JNI, SDK, NDK, ART or system-specific intrinsics at runtime. Caller owns a 336-byte-ish state and supplies input bytes. Streaming updates are bounded per call to `rafb_u32` and can be repeated; final consumes the state. The `equal` helper compares a full 32-byte digest without early exit. It does **not** authenticate the source or sign an APK: the expected digest and provenance must come through a trusted independent authority.

The 32-bit rotation implementation and fixed Keccak Pi mapping avoid ARM32 compiler helper references (`__aeabi_llsl`, `__aeabi_llsr`, `__aeabi_uidiv`). The host gate executes six known-answer vectors (empty, abc, 135/136/137/1000 bytes), fragmented streaming updates, invalid-state negatives, 56 one-bit negative fixtures, ASan/UBSan, and eight cross-target object compilations with zero undefined symbols.

Run `sh rafcode/build/ci/verify_sha3.sh`. The producer single existing scoped CI invokes this from `verify_abi.sh`, alongside Watchdog². Zero undefined ELF object symbols is not proof of device performance, constant-time machine instructions or cryptographic certification. FIPS implementation conformance and independent security audit require separate evaluation.

UPSTREAM_NINJA untouched; `rafcode/command` and `rafcode/l0` untouched. Outbound license `TOKEN_VAZIO`, TMD expansion `TOKEN_VAZIO`, claim_allowed=false, no merge without independent review.
