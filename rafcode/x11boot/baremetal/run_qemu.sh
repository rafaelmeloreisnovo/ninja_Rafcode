#!/bin/sh
# Optional emulator evidence; no mock is accepted as physical hardware proof.
set -eu
cd "$(dirname "$0")"
: "${QEMU_TIMEOUT:=5}"
ran=0
missing=0
for abi in armv7 aarch64; do
  case "$abi" in
    armv7) emulator=qemu-system-arm; cpu=cortex-a15 ;;
    aarch64) emulator=qemu-system-aarch64; cpu=cortex-a57 ;;
  esac
  if ! command -v "$emulator" >/dev/null 2>&1; then
    echo "NOT_RUN $abi: $emulator absent"; missing=1; continue
  fi
  tmp=$(mktemp)
  result=0
  timeout "${QEMU_TIMEOUT}s" "$emulator" -M virt -cpu "$cpu" -m 128M -nographic \
    -monitor none -serial stdio -no-reboot -kernel "boot-$abi.elf" >"$tmp" 2>&1 || result=$?
  if [ "$result" -eq 124 ] && grep -q "RAFCODE Phi bare metal |" "$tmp"; then
    echo "PASS_SCOPED QEMU UART $abi (emulated serial only)"
    ran=$((ran + 1))
  else
    echo "FAIL_QEMU $abi rc=$result"
    cat "$tmp"
    rm -f "$tmp"
    exit 1
  fi
  rm -f "$tmp"
done
if [ "$ran" -eq 0 ]; then
  echo 'BLOCKED_NO_QEMU: no emulation was executed, no success claim'
  exit 77
fi
if [ "$missing" -ne 0 ]; then
  echo 'PARTIAL_NOT_RUN: at least one ABI not emulated'
  exit 77
fi
