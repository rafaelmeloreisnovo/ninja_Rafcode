#!/bin/sh
# Optional emulator evidence; no mock is accepted as physical hardware proof.
set -eu
cd "$(dirname "$0")"
: "${QEMU_TIMEOUT:=5}"
for abi in armv7 aarch64; do
  case "$abi" in
    armv7) emulator=qemu-system-arm; cpu=cortex-a15 ;;
    aarch64) emulator=qemu-system-aarch64; cpu=cortex-a57 ;;
  esac
  if ! command -v "$emulator" >/dev/null 2>&1; then
    echo "NOT_RUN $abi: $emulator absent"; continue
  fi
  tmp=$(mktemp)
  result=0
  timeout "${QEMU_TIMEOUT}s" "$emulator" -M virt -cpu "$cpu" -m 128M -nographic \
    -monitor none -serial stdio -no-reboot -kernel "boot-$abi.elf" >"$tmp" 2>&1 || result=$?
  if [ "$result" -eq 124 ] && grep -q "RAFCODE Phi bare metal |" "$tmp"; then
    echo "PASS_SCOPED QEMU UART $abi (emulated serial only)"
  else
    echo "FAIL_QEMU $abi rc=$result"
    cat "$tmp"
    rm -f "$tmp"
    exit 1
  fi
  rm -f "$tmp"
done
