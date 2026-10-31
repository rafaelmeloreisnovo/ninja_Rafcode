#!/bin/sh
# Hosted verification only: FIPS202 SHA3-256 authorial freestanding implementation.
set -eu
cd "$(dirname "$0")/../../.."
for t in cc clang nm readelf sha256sum mktemp; do
 command -v "$t" >/dev/null 2>&1 || { echo "BLOCKED_TOOL=$t" >&2;exit 1; }
done
umask 077
OUT=$(mktemp -d "/tmp/rafb-sha3.XXXXXXXX")
trap 'rm -rf -- "$OUT"' EXIT HUP INT TERM
H=rafcode/build/include
S=rafcode/build/hash
cc -std=c11 -O2 -ffreestanding -fno-builtin -Wall -Wextra -Werror -pedantic  -I"$H" -I"$S" "$S/rafb_sha3.c" "$S/test_rafb_sha3.c" -o "$OUT/sha3-host"
"$OUT/sha3-host"
cc -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer  -Wall -Wextra -Werror -pedantic -I"$H" -I"$S"  "$S/rafb_sha3.c" "$S/test_rafb_sha3.c" -o "$OUT/sha3-sanitized"
"$OUT/sha3-sanitized"
printf 'SHA3_HOST_KAT=PASS SHA3_ASAN_UBSAN=PASS SHA3_BIT_NEGATIVES=56\n'
n=0
for target in armv7a-linux-androideabi24 aarch64-linux-android24  i686-linux-android24 x86_64-linux-android24 armv7m-none-eabi  aarch64-none-elf riscv32-unknown-elf riscv64-unknown-elf; do
 clang --target="$target" -std=c11 -Oz -Wall -Wextra -Werror  -ffreestanding -fno-builtin -fno-pic -fno-pie -fno-stack-protector  -nostdinc -I"$H" -I"$S" -c "$S/rafb_sha3.c" -o "$OUT/$n.o"
 [ -z "$(nm -u "$OUT/$n.o")" ] || { echo "SHA3_EXTERN_BLOCKED=$target" >&2;exit 1; }
 readelf -h "$OUT/$n.o" | grep -F 'REL (Relocatable file)' >/dev/null || exit 1
 printf 'SHA3_OBJECT=PASS target=%s\n' "$target"
 n=$((n+1))
done
printf 'SHA3_SOURCE_SHA256='
sha256sum "$S/rafb_sha3.c" | cut -d' ' -f1
printf 'HASH=TESTED_SIGNATURE=NOT_INCLUDED SOURCE_IDENTITY=HOST_AUTHORITY_REQUIRED CLAIM_ALLOWED=false\n'
