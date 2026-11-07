# RAFCODE Phi — μWRITE append-only correction V2

μID: NINJA-RAFCODE-BM-20261107-02
timestamp: 2026-11-07 (America/Sao_Paulo)
source/ref: https://github.com/rafaelmeloreisnovo/ninja_Rafcode/pull/11
parent: 2026-11-07_BM_V1_RECEIPT.md
kind: CORRECTION + FAIL_CLOSED_GATE
supersedes_field: optional QEMU runner's no-execution exit status
delta: If no QEMU emulator is installed or any target is not executed, runner returns exit code 77 with BLOCKED_NO_QEMU/PARTIAL_NOT_RUN rather than success status 0.
routes: P=rafcode/x11boot/baremetal; C=QEMU optional test; E=local mocked absence test; I=001_PONTEIRO
evidence: local run produced NOT_RUN for ARMv7 and AArch64 and process exit 77. QEMU execution itself NOT_RUN.
gap: target runtime/emulator/hardware and DTB UART discovery remain TOKEN_VAZIO/NOT_RUN.
next: execute same script in pinned QEMU environment, retain stdout/exit code/firmware profile receipts.
claim_allowed: false for hardware boot and full X11.
rollback: leave PR #11 draft and retain predecessor receipt, no file deletion.
R3: <absence gate blocks, emulation NOT_RUN, real QEMU boot checks>
