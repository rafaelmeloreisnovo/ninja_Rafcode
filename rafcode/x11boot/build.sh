#!/bin/sh
# Hosted assembler/linker only; not linked into ELF.
set -eu
cd "$(dirname "$0")"
: "${CC:=clang}"
: "${LD_FLAVOR:=lld}"
for spec in 'x86_64:x86_64-linux-gnu' 'aarch64:aarch64-linux-gnu' 'armv7:armv7a-linux-gnueabi'; do
  abi=${spec%%:*}
  target=${spec#*:}
  "$CC" --target="$target" -Wall -Wextra -Werror -x assembler-with-cpp -nostdlib -static \
    -fno-pie -fuse-ld="$LD_FLAVOR" -Wl,-e,_start \
    -Wl,--build-id=none -Wl,-s -Wl,-z,noexecstack \
    -Wl,--gc-sections -Wl,--no-undefined -Wl,--fatal-warnings \
    x11boot.S -o "x11boot-$abi.elf"
done
