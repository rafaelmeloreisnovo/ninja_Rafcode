#!/bin/sh
# HOST-ONLY reproducible verifier: no target SDK/NDK, Gradle, runtime libs.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
cd "$ROOT"
for cmd in cc clang readelf nm sha256sum mktemp; do
  command -v "$cmd" >/dev/null 2>&1 || { echo "BLOCKED: $cmd" >&2;exit 1; }
done
umask 077
OUT=$(mktemp -d "${TMPDIR:-/tmp}/rafb-abi.XXXXXXXX")
trap 'rm -rf -- "$OUT"' EXIT HUP INT TERM
CFLAGS='-std=c11 -O2 -Wall -Wextra -Werror -pedantic -ffreestanding -fno-builtin -fno-stack-protector'
cc $CFLAGS -Irafcode/build/include -Irafcode/build/elf \
  rafcode/build/elf/rafb_elf.c rafcode/build/elf/test_rafb_elf.c -o "$OUT/test-elf"
"$OUT/test-elf"
printf 'ELF_PARSER_TEST=PASS\n'
# Each .o must have ELF architecture data consistent with the target and no undefined symbols.
i=0
for spec in \
  'armv7a-linux-androideabi24:ARM:ELF32' \
  'aarch64-linux-android24:AArch64:ELF64' \
  'i686-linux-android24:Intel 80386:ELF32' \
  'x86_64-linux-android24:Advanced Micro Devices X86-64:ELF64' \
  'armv7m-none-eabi:ARM:ELF32' \
  'aarch64-none-elf:AArch64:ELF64' \
  'riscv32-unknown-elf:RISC-V:ELF32' \
  'riscv64-unknown-elf:RISC-V:ELF64'; do
  target=${spec%%:*};remain=${spec#*:};machine=${remain%:*};class=${spec##*:}
  clang --target="$target" -std=c11 -Oz -Wall -Wextra -Werror \
    -ffreestanding -fno-builtin -fno-stack-protector -fno-pic -fno-pie \
    -nostdinc -Irafcode/build/include -Irafcode/build/elf \
    -c rafcode/build/elf/rafb_elf.c -o "$OUT/$i.o"
  readelf -h "$OUT/$i.o" > "$OUT/$i.header"
  grep -F "Machine:                           $machine" "$OUT/$i.header" >/dev/null || {
     echo "ABI_MISMATCH $target machine" >&2;exit 1; }
  grep -F "Class:                             $class" "$OUT/$i.header" >/dev/null || {
     echo "ABI_MISMATCH $target class" >&2;exit 1; }
  [ -z "$(nm -u "$OUT/$i.o")" ] || { echo "EXTERNAL_SYMBOL $target" >&2;exit 1; }
  printf 'ELF_OBJECT PASS abi=%s target=%s sha256=' "$i" "$target"
  sha256sum "$OUT/$i.o" | cut -d' ' -f1
  i=$((i+1))
done
# Actual linkable Android .so objects: no libc and no DT_NEEDED. Not an Android APK.
for spec in 'armeabi-v7a:armv7a-linux-androideabi24:ARM:ELF32' 'arm64-v8a:aarch64-linux-android24:AArch64:ELF64'; do
  abi=${spec%%:*};p=${spec#*:};target=${p%%:*}
  clang --target="$target" -Oz -ffreestanding -fPIC -fno-builtin -fno-stack-protector \
    -nostdinc -Irafcode/build/include -Irafcode/build/elf \
    -c rafcode/build/src/rafb.c -o "$OUT/core-$abi.o"
  clang --target="$target" -Oz -ffreestanding -fPIC -fno-builtin -fno-stack-protector \
    -nostdinc -Irafcode/build/include -Irafcode/build/elf \
    -c rafcode/build/elf/rafb_elf.c -o "$OUT/elf-$abi.o"
  clang --target="$target" -nostdlib -fuse-ld=lld -shared \
    -Wl,--no-undefined -Wl,-soname,librafb.so -Wl,-z,relro,-z,now \
    "$OUT/core-$abi.o" "$OUT/elf-$abi.o" -o "$OUT/librafb-$abi.so"
  readelf -h "$OUT/librafb-$abi.so" | grep -F 'DYN (Shared object file)' >/dev/null || exit 1
  if readelf -d "$OUT/librafb-$abi.so" | grep -F 'NEEDED'; then
    echo "DYNAMIC_DEPENDENCY_REJECTED abi=$abi" >&2;exit 1
  fi
  printf 'ANDROID_SHARED_ELF PASS abi=%s size_bytes=' "$abi"
  wc -c < "$OUT/librafb-$abi.so"
done
printf 'SCOPE=BUILD_AND_ELF_INSPECTION_ONLY RUNTIME=TOKEN_VAZIO APK=TOKEN_VAZIO\n'
# The single existing producer-l0 workflow invokes this scoped extension.
sh rafcode/build/ci/verify_watchdog.sh
sh rafcode/build/ci/verify_sha3.sh
sh rafcode/build/ci/verify_bundle.sh
