#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
OUT="${TMPDIR:-/tmp}/rafn-inline-asm-$$"
mkdir -p "$OUT"
trap 'rm -rf "$OUT"' EXIT HUP INT TERM
for mode in asm scalar; do
  if [ "$mode" = scalar ]; then EXTRA=-DRAFN_FORCE_SCALAR=1; else EXTRA=; fi
  cc -std=c11 -O2 -Wall -Wextra -Werror -pedantic -ffreestanding -fno-builtin \
     $EXTRA -Irafcode/ninja_asm/include \
     rafcode/ninja_asm/src/rafn.c rafcode/ninja_asm/tests/test_rafn.c -o "$OUT/$mode"
  "$OUT/$mode"
  printf 'HOST_DIFFERENTIAL_%s=PASS\n' "$mode"
done
for t in armv7a-linux-androideabi24 aarch64-linux-android24 \
         i686-linux-android24 x86_64-linux-android24 \
         armv7m-none-eabi aarch64-none-elf \
         riscv32-unknown-elf riscv64-unknown-elf; do
  clang --target="$t" -std=c11 -Oz -Wall -Wextra -Werror -ffreestanding \
    -fno-builtin -fno-stack-protector -nostdinc -fno-pic -fno-pie \
    -Irafcode/ninja_asm/include -c rafcode/ninja_asm/src/rafn.c -o "$OUT/$t.o"
  readelf -h "$OUT/$t.o" >/dev/null
  if [ -n "$(nm -u "$OUT/$t.o")" ]; then echo "UNRESOLVED $t" >&2; exit 1; fi
  printf 'OBJECT_%s=PASS RUNTIME=NOT_RUN\n' "$t"
done
