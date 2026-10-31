#!/bin/sh
# Authorial C11 watchdog-of-watchdog scoped gate, hosted test driver.
set -eu
cd "$(dirname "$0")/../../.."
for cmd in cc clang nm readelf sha256sum mktemp; do
 command -v "$cmd" >/dev/null 2>&1 || { printf 'BLOCKED_TOOL=%s\n' "$cmd" >&2;exit 1; }
done
umask 077
OUT=$(mktemp -d "${TMPDIR:-/tmp}/rafb-wd2.XXXXXXXX")
trap 'rm -rf -- "$OUT"' EXIT HUP INT TERM
H=rafcode/build/include
W=rafcode/build/watchdog
FLAGS='-std=c11 -O2 -Wall -Wextra -Werror -pedantic -ffreestanding -fno-builtin -fno-stack-protector'
cc $FLAGS -I"$H" -I"$W" "$W/rafb_wd2.c" "$W/test_rafb_wd2.c" -o "$OUT/test-wd2"
"$OUT/test-wd2"
cc $FLAGS -I"$H" -I"$W" "$W/rafb_wd2.c" "$W/rafb_wd2_bridge.c" "$W/test_rafb_wd2_bridge.c" -o "$OUT/test-bridge"
"$OUT/test-bridge"
printf 'WD2_TESTS=PASS BRIDGE=PASS BIT_MUTATIONS=56\n'
cc -std=c11 -O1 -g -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined \
  -fno-omit-frame-pointer -I"$H" -I"$W" "$W/rafb_wd2.c" "$W/test_rafb_wd2.c" -o "$OUT/sanitized"
"$OUT/sanitized"
printf 'WD2_ASAN_UBSAN=PASS\n'
n=0
for target in armv7a-linux-androideabi24 aarch64-linux-android24 \
 i686-linux-android24 x86_64-linux-android24 armv7m-none-eabi \
 aarch64-none-elf riscv32-unknown-elf riscv64-unknown-elf; do
 for part in rafb_wd2 rafb_wd2_bridge; do
  clang --target="$target" -std=c11 -Oz -Wall -Wextra -Werror -ffreestanding \
   -fno-builtin -fno-stack-protector -fno-pic -fno-pie -nostdinc \
   -I"$H" -I"$W" -c "$W/$part.c" -o "$OUT/$n-$part.o"
 done
 clang --target="$target" -nostdlib -fuse-ld=lld -r \
  "$OUT/$n-rafb_wd2.o" "$OUT/$n-rafb_wd2_bridge.o" -o "$OUT/$n-combined.o"
 [ -z "$(nm -u "$OUT/$n-combined.o")" ] || { echo "UNRESOLVED_TARGET=$target" >&2;exit 1; }
 printf 'WD2_OBJECT=PASS target=%s\n' "$target"
 n=$((n+1))
done
for spec in armeabi-v7a:armv7a-linux-androideabi24:ELF32 arm64-v8a:aarch64-linux-android24:ELF64; do
 abi=${spec%%:*}
 rest=${spec#*:}
 target=${rest%%:*}
 class=${spec##*:}
 for part in rafb_wd2 rafb_wd2_bridge; do
  clang --target="$target" -Oz -ffreestanding -fPIC -fno-builtin \
   -fno-stack-protector -nostdinc -I"$H" -I"$W" \
   -c "$W/$part.c" -o "$OUT/$abi-$part.pic.o"
 done
 clang --target="$target" -nostdlib -fuse-ld=lld -shared \
  -Wl,--no-undefined -Wl,-soname,librafb_wd2.so -Wl,-z,relro,-z,now \
  "$OUT/$abi-rafb_wd2.pic.o" "$OUT/$abi-rafb_wd2_bridge.pic.o" -o "$OUT/lib-$abi.so"
 readelf -h "$OUT/lib-$abi.so" | grep -F "Class:                             $class" >/dev/null || exit 1
 readelf -h "$OUT/lib-$abi.so" | grep -F 'DYN (Shared object file)' >/dev/null || exit 1
 if readelf -d "$OUT/lib-$abi.so" | grep -F 'NEEDED'; then echo "BLOCKED_DT_NEEDED=$abi" >&2;exit 1;fi
 printf 'WD2_SHARED=PASS abi=%s size=' "$abi"
 wc -c < "$OUT/lib-$abi.so"
done
printf 'WD2_SOURCE_SHA256='
sha256sum "$W/rafb_wd2.c" | cut -d' ' -f1
printf 'DEVICE=TOKEN_VAZIO INDEPENDENT_HW=TOKEN_VAZIO CLAIM_ALLOWED=false\n'
