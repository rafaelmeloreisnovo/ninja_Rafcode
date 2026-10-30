#!/bin/sh
# Hosted test/compile driver. Fail closed when toolchain or verification is absent.
set -eu
cd "$(dirname "$0")/../.."
umask 077
OUT=/tmp/rafb-gate-check
mkdir -p "$OUT"
cc -std=c11 -O2 -Wall -Wextra -Werror -pedantic -ffreestanding -fno-builtin \
 -Irafcode/build/include rafcode/build/src/rafb.c rafcode/build/tests/test_rafb.c -o "$OUT/test"
"$OUT/test"
printf 'HOST_TEST=PASS\n'
i=0
for target in armv7a-linux-androideabi24 aarch64-linux-android24 \
  i686-linux-android24 x86_64-linux-android24 armv7m-none-eabi \
  aarch64-none-elf riscv32-unknown-elf riscv64-unknown-elf; do
  clang --target="$target" -std=c11 -O2 -Wall -Wextra -Werror \
    -ffreestanding -fno-builtin -fno-stack-protector -fno-pic -fno-pie \
    -nostdinc -Irafcode/build/include -c rafcode/build/src/rafb.c -o "$OUT/$i.o"
  readelf -h "$OUT/$i.o" > "$OUT/$i.elf-header"
  if [ -n "$(nm -u "$OUT/$i.o")" ]; then
    printf 'UNRESOLVED_SYMBOL profile=%s target=%s\n' "$i" "$target" >&2; exit 1
  fi
  printf 'OBJECT_COMPILE=PASS profile=%s target=%s runtime=TOKEN_VAZIO\n' "$i" "$target"
  i=$((i+1))
done
printf 'SOURCE_SHA256='
sha256sum rafcode/build/src/rafb.c
printf 'HOST_ARTIFACTS=%s\n' "$OUT"
