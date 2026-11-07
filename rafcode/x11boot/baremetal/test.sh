#!/bin/sh
# Build/ELF/ABI gates. No hardware execution claimed without an emulator/board.
set -eu
cd "$(dirname "$0")"
sh ./build.sh
: "${CC:=clang}"
for spec in 'armv7:armv7a-none-eabi:ARM' 'aarch64:aarch64-none-elf:AArch64'; do
  abi=${spec%%:*}
  rest=${spec#*:}
  target=${rest%%:*}
  machine=${rest#*:}
  f="boot-$abi.elf"
  readelf -h "$f" | grep -q 'Type:.*EXEC'
  readelf -h "$f" | grep -q "Machine:.*$machine"
  readelf -h "$f" | grep -q 'Entry point address:.*0x40080000'
  readelf -l "$f" | grep -q 'LOAD'
  if readelf -l "$f" | grep -q 'INTERP\|DYNAMIC'; then echo "FAIL: OS loader segment $f"; exit 1; fi
  if readelf -d "$f" 2>/dev/null | grep -q NEEDED; then echo "FAIL: linked library $f"; exit 1; fi
  if readelf -r "$f" | grep -qE 'R_ARM_|R_AARCH64_'; then echo "FAIL: unapplied relocation $f"; exit 1; fi
  if nm -u "$f" | grep -q .; then echo "FAIL: unresolved symbol $f"; exit 1; fi
  if llvm-objdump -d "$f" | grep -Eiq '\b(svc|syscall|hvc|smc)\b'; then echo "FAIL: trap instruction $f"; exit 1; fi
  printf '#include "raf_bm_contract.h"\n' | "$CC" --target="$target" -x c -std=c11 -ffreestanding -nostdinc -fsyntax-only -Wall -Wextra -Werror -Iinclude -
  echo "PASS: ELF/ABI/runtime-dependency structural gates $abi"
done
# Falsifier: cannot silently build a board with unspecified UART memory map.
if "$CC" --target=aarch64-none-elf -x assembler-with-cpp -c uart.S -o /dev/null 2>/dev/null; then
  echo 'FAIL: missing RAF_UART_BASE was not rejected'; exit 1
fi
# Falsifier: don't silently reinterpret an x86 target as ARM board.
if "$CC" --target=x86_64-none-elf -x assembler-with-cpp -DRAF_UART_BASE=0x09000000 -c uart.S -o /dev/null 2>/dev/null; then
  echo 'FAIL: unsupported target was not rejected'; exit 1
fi
echo 'PASS_SCOPED: no loader/libc/unresolved calls/trap opcodes; QEMU/hardware NOT_RUN'
