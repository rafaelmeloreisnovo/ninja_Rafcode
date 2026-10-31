# RAFCODE authorial WD2 — staged integration

This directory is strictly under `rafcode/build/**`. The inherited Ninja source and independent `rafcode/command` and `rafcode/l0` nuclei are out of scope. Pure C11 freestanding, caller-owned memory, caller-provided monotonic ticks, zero libc/syscalls/heap/JNI/ART/NDK. Watchdog-1 binds observed worker sequence; independently scheduled watchdog-2 detects stale supervisor/worker and latches HALTED/ISOLATED. The bridge maps failures to build gate FAILSAFE, never silently retries or executes commands.

No physical independent scheduler, tamper-resistant authentication, hardware reset, APK signing, device execution or external authorization is supplied by this library. The rearm constant is not a secret. Outbound RAFCODE license and TMD expansion remain TOKEN_VAZIO.

Gate: `sh rafcode/build/ci/verify_watchdog.sh`; run only in an authorized hosted CI context, using compilers solely as **build tools**. Use the source SHA, exact CI HEAD, emitted ELF/receipt and provider output to decide scoped PASS. Existing producer gate must remain green. ROLLBACK=close draft PR; no merge/deploy. SOURCE!=ARTIFACT!=EXECUTION!=EVIDENCE!=CLAIM; claim_allowed=false.
