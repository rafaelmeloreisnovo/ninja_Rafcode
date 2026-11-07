#!/bin/sh
# Build-tool inputs are external; only emitted ELF/raw binary has no runtime libs.
set -eu
cd "$(dirname "$0")"
: "${CC:=clang}"
: "${OBJCOPY:=llvm-objcopy}"
for spec in 'armv7:armv7a-none-eabi' 'aarch64:aarch64-none-elf'; do
  abi=${spec%%:*}
  target=${spec#*:}
  for name in entry uart main; do
    "$CC" --target="$target" -x assembler-with-cpp \
      -DRAF_UART_BASE=0x09000000 -Wa,--fatal-warnings -Wall -Wextra -Werror \
      -c "$name.S" -o "$name-$abi.o"
  done
  "$CC" --target="$target" -nostdlib -static -fno-pie -fuse-ld=lld \
    -Wl,-T,link.ld -Wl,--no-undefined -Wl,--fatal-warnings \
    -Wl,--build-id=none -Wl,--gc-sections -Wl,-z,max-page-size=4096 \
    "entry-$abi.o" "uart-$abi.o" "main-$abi.o" -o "boot-$abi.elf"
  "$OBJCOPY" -O binary "boot-$abi.elf" "boot-$abi.bin"
done
