#!/bin/sh
# Hosted-only tests, not ELF runtime dependencies.
set -eu
cd "$(dirname "$0")"
sh ./build.sh
for f in x11boot-*.elf; do
  readelf -h "$f" | grep -q 'Type:.*EXEC'
  if readelf -l "$f" | grep -q INTERP; then echo "FAIL: PT_INTERP $f"; exit 1; fi
  if readelf -d "$f" 2>/dev/null | grep -q NEEDED; then echo "FAIL: DT_NEEDED $f"; exit 1; fi
  echo "PASS: static ELF structure $f"
done
printf 'ABC\n' | ./x11boot-x86_64.elf | grep -q 'ABC'
python3 ./test_x11.py
printf '%s\n' 'PASS: x86_64 native; ARMv7/AArch64 compile-and-inspect only.'
