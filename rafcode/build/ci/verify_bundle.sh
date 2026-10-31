#!/bin/sh
# One freestanding RAFCODE authorial L0 runtime library per Android ABI.
# Builds only native ELF objects, never APK/manifest/ART/JNI or signs anything.
set -eu
cd "$(dirname "$0")/../../.."
for cmd in clang readelf nm sha256sum mktemp; do
 command -v "$cmd" >/dev/null 2>&1 || { echo "BLOCKED_TOOL=$cmd" >&2;exit 1; }
done
umask 077
OUT=$(mktemp -d "/tmp/rafb-bundle.XXXXXXXX")
trap 'rm -rf -- "$OUT"' EXIT HUP INT TERM
for spec in armeabi-v7a:armv7a-linux-androideabi24:ELF32:ARM arm64-v8a:aarch64-linux-android24:ELF64:AArch64; do
 abi=${spec%%:*}
 rest=${spec#*:}
 target=${rest%%:*}
 rest=${rest#*:}
 class=${rest%%:*}
 machine=${rest#*:}
 for part in core elf wd2 bridge hash; do
  case "$part" in
   core) src=rafcode/build/src/rafb.c ;;
   elf) src=rafcode/build/elf/rafb_elf.c ;;
   wd2) src=rafcode/build/watchdog/rafb_wd2.c ;;
   bridge) src=rafcode/build/watchdog/rafb_wd2_bridge.c ;;
   hash) src=rafcode/build/hash/rafb_sha3.c ;;
  esac
  clang --target="$target" -std=c11 -Oz -Wall -Wextra -Werror \
   -ffreestanding -fPIC -fno-builtin -fno-stack-protector -nostdinc \
   -Irafcode/build/include -Irafcode/build/elf -Irafcode/build/watchdog \
   -Irafcode/build/hash -c "$src" -o "$OUT/$abi-$part.o"
 done
 clang --target="$target" -nostdlib -fuse-ld=lld -shared \
  -Wl,--no-undefined -Wl,-soname,librafb.so -Wl,-z,relro,-z,now \
  "$OUT/$abi-core.o" "$OUT/$abi-elf.o" "$OUT/$abi-wd2.o" \
  "$OUT/$abi-bridge.o" "$OUT/$abi-hash.o" -o "$OUT/$abi-librafb.so"
 readelf -h "$OUT/$abi-librafb.so" | grep -F "Class:                             $class" >/dev/null || exit 1
 readelf -h "$OUT/$abi-librafb.so" | grep -F "Machine:                           $machine" >/dev/null || exit 1
 readelf -h "$OUT/$abi-librafb.so" | grep -F 'DYN (Shared object file)' >/dev/null || exit 1
 if readelf -d "$OUT/$abi-librafb.so" | grep -F 'NEEDED'; then
  echo "BLOCKED_DYNAMIC_DEPENDENCY=$abi" >&2;exit 1
 fi
 [ -z "$(nm -D -u "$OUT/$abi-librafb.so")" ] || { echo "BLOCKED_UNDEFINED=$abi" >&2;exit 1; }
 printf 'RAFB_BUNDLE=PASS abi=%s bytes=' "$abi"
 wc -c < "$OUT/$abi-librafb.so"
 printf 'RAFB_BUNDLE_SHA256 abi=%s digest=' "$abi"
 sha256sum "$OUT/$abi-librafb.so" | cut -d' ' -f1
done
printf 'LIBRARY_ONLY=PASS NO_APK=TRUE HARDWARE=TOKEN_VAZIO SIGNATURE=TOKEN_VAZIO CLAIM_ALLOWED=false\n'
