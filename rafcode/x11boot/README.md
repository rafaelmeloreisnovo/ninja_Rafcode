# RAFCODE Φ — ELF bootstrap / VT stream / X11 wire V1

**STATUS: PASS_SCOPED on Linux x86-64, COMPILE_INSPECTED on ARMv7/AArch64; NOT an Xterm emulator or bare-metal bootloader.**

This is a deliberately separate, original `rafcode/**` experiment. It does **not**
modify the inherited Ninja code and does **not** copy a Ninja, Xlib, Xterm,
BOOTSTRAPsys, Android or C-runtime implementation. Protocol numbers and ELF/ABI
identifiers are interoperable public facts, not borrowed implementation code.

## What actually runs

- A single assembly-with-C-preprocessor source, `x11boot.S`, selects one of
  Linux x86-64, Linux AArch64 or Linux ARMv7 EABI at compile time.
- An ELF `_start` performs Linux `read`, `write`, `exit`, `socket`, `connect` and
  `close` by **direct kernel syscalls**. No `main`, CRT, libc, malloc, heap, C
  globals, external data symbols, interpreter or dynamic runtime library.
- A 2064-byte fixed stack frame carries a bounded X11 setup response or a
  2048-byte VT pass-through chunk. Short writes are handled explicitly.
- A fixed, unauthenticated Unix-domain connection to `/tmp/.X11-unix/X0` sends
  the 12-byte X11 little-endian setup request, then consumes the complete reply
  if the status is successful and the additional body length is 32..2048 bytes.
  Rejected, unsupported or unavailable setups fall back to the VT byte stream.
- The VT stream passes bytes unchanged between stdin and stdout; it is not an
  ANSI/VT parser, framebuffer, terminal line editor, PTY multiplexer or
  X11 graphics backend.

**X11 is an external protocol/server dependency.** Eliminating Xlib and heap
cannot eliminate the server, OS socket support, authentication or display
hardware. The current unauthenticated probe will commonly be denied. The
`X11_FRAME_OK` line means only the basic setup frame was received and bounded,
not that a screen was created, rendered or authenticated for general use.

## Build (toolchain is a build-time dependency)

```sh
sh rafcode/x11boot/build.sh
sh rafcode/x11boot/test.sh
```

Hosted `clang`, `lld`, `readelf`, `python3` (regression fixture) and a POSIX
shell are needed for these build/tests. They are not linked to the candidate
ELFs. `build.sh` uses `--target`, `-nostdlib`, `-static`, `-fno-pie`, a direct
`_start`, no build ID, stripped symbols, `--gc-sections`, `--no-undefined`,
`--fatal-warnings`, NX-stack and compiler diagnostics as errors. Flags affect
assembly/linkage; source comments do not alter emitted instructions directly.
Each output is a distinct ELF for its architecture; a multi-ABI fat ELF is not
claimed. Binaries are build outputs and intentionally **not committed**.

| Target | Output | Native execution evidence |
|---|---|---|
| x86-64 Linux | `x11boot-x86_64.elf` | native host + simulated X11 wire PASS |
| ARMv7 EABI Linux | `x11boot-armv7.elf` | ELF build/readelf only; device NOT_RUN |
| AArch64 Linux | `x11boot-aarch64.elf` | ELF build/readelf only; device NOT_RUN |

The tests exercise a split X11 response, negative setup, oversized length,
opaque NUL/0xff bytes, a payload larger than 2048 bytes, no PT_INTERP and no
DT_NEEDED. The fake X11 server only validates framing; it is not a real server.

## Gaps and production gates

1. **Not Xterm/X11 window rendering:** no Xauthority cookie negotiation,
   runtime `DISPLAY` socket selection, setup screen parsing, resource IDs,
   CreateWindow, MapWindow, text drawing, glyph/font handling, X events or PTY.
2. **Not bare metal:** Linux syscall numbers and ELF process loader are
   environment requirements. Bare-metal needs board-specific reset entry,
   physical memory map, UART/framebuffer drivers, interrupt handling and ABI.
3. **Not Android-validated:** native ARM EABI ELF compilation proves neither
   Android loader permission nor Termux app compatibility. Physical tests for
   Motorola ARMv7, AArch64 Android and general X11 display remain `NOT_RUN`.
4. **Hardening:** fixed socket path (17 bytes), no Xauthority, no nonblocking
   IO/timeouts, no event loop, no signal/EINTR retries, no deliberate resource
   budgets beyond local buffer sizing. Hostile servers might stall reads.
5. **Licenses:** upstream Ninja `COPYING` remains untouched. RAFCODE original
   code outbound rights remain `LICENSE_STATE=TOKEN_VAZIO`,
   `OUTBOUND_LICENSE_GRANT=false` until owner review. No license is implied.

## Evidence boundary and rollback

`SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM`

`claim_allowed=false` for production Xterm, general X11 compatibility,
Android APK installation, cross-ABI execution and physical bare-metal operation.

This change is restricted to `rafcode/x11boot/**` on a feature branch; rollback
is closing the PR / not merging that branch, without touching upstream files.
`AGENTS.md` and the official Ninja source remain unchanged.

TMD_SCOPE=rafcode/x11boot
TMD_SOURCE=original RAFCODE assembly source and test fixture
TMD_DELTA=ELF bootstrap + VT byte transport + X11 setup-frame probe
TMD_EVIDENCE=local x86-64 tests and cross-ELF structural checks only
TMD_STATE=TOKEN_VAZIO

R3 = <ELF build and host tests PASS_SCOPED, X11 GUI/Android/bare-metal TOKEN_VAZIO, extend Xauthority/graphics with bounded adversarial tests>
## Bare-metal sibling (isolated, not the X11 client)

The original `x11boot.S` described above remains the Linux-syscall ELF candidate.
A new independent `baremetal/` module tree implements an ARMv7/AArch64 no-syscall
PL011 serial boot experiment; see [its source and evidence](baremetal/README.md)
and the [append-only pointer](baremetal/001_PONTEIRO/2026-11-07_BM_V1_RECEIPT.md).
The two binaries have different execution contracts and must not be conflated.
No evidence of QEMU or physical ARM boot is claimed by the bare-metal code.
