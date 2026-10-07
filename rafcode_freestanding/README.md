# RAFCODE Freestanding L0

A small, newly authored RAFCODE nucleus kept separate from inherited Ninja code.

## Boundary

`SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM`.

This directory does **not** claim that Ninja itself is freestanding or newly authored. Ninja remains inherited upstream code under the repository's Apache-2.0 `COPYING`. Files in this directory are new authored contributions by Rafael Melo Reis and are also licensed under Apache-2.0 so the repository keeps one compatible redistribution regime.

## L0 contract

The core is deliberately narrow:

- no C standard-library headers;
- no libc calls;
- no heap or allocator;
- no syscalls;
- no recursion;
- no source-level loop in the L0 observer;
- no external runtime symbol required by the compiled core object;
- explicit `provided` bit: `ABSENT != EMPTY != NUL != SPACE != BYTE != SEQUENCE != INVALID`;
- fixed 16-byte result: a flag word plus compact metadata;
- caller-owned input/output; no ownership transfer;
- one guarded byte read only when `{provided=1,size>0,data!=0}`;
- zero context, attention, learned weights, vocabulary, normalization, or history semantics.

`TOKEN_VAZIO != 0`: an unknown evidence state must not be encoded as numeric zero.

## Bit layout

Dynamic flags occupy bits 0..11. Contract/profile flags occupy bits 32..40. `meta[7:0]` holds the first byte when readable and `meta[11:8]` holds the whole-input state.

The fixed layout is intended to reduce ABI ambiguity and layering. It is a contract, not a claim that every compiler/ISA will emit identical machine code.

## Toolchain boundary

A compiler/linker is a **build/verification provider**, not a runtime dependency of the L0 object. A representative verification command is:

```sh
clang -std=c11 -O2 -ffreestanding -fno-builtin -fno-stack-protector \
  -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-pic -fno-pie \
  -Iinclude -c src/raf_l0.c -o raf_l0.o
```

Then inspect rather than assume:

```sh
nm -u raf_l0.o
objdump -dr raf_l0.o
```

For an ELF layout probe:

```sh
clang -c arch/x86_64/raf_leaf.S -o raf_leaf.o
ld.lld -static -T link/raf_l0.ld raf_l0.o raf_leaf.o -o raf_l0.elf
readelf -h -S -l -d raf_l0.elf
nm -u raf_l0.elf
```

The optional x86-64 leaf primitive has canonical bytes `48 89 f8 48 0f c8 c3` (`mov rdi,rax; bswap rax; ret`) and is intentionally not a dependency of `raf_l0_observe`.

## Horizons

Keep each future path independently falsifiable:

1. **C L0** — semantics and bit ABI first.
2. **ISA leaves** — add only where disassembly proves lower friction/cost; never replace portable semantics without equivalence tests.
3. **Link layout** — explicit sections, no interpreter/dynamic runtime at the freestanding artifact boundary.
4. **Preprocessor contract** — compiler capabilities are explicit gates, not silent fallbacks.
5. **Block kernels** — fixed-size kernels first; tails stay outside the kernel until they have their own proof.
6. **Physical execution** — separate gate. Build/codegen PASS is not device PASS.

## Non-claims

- Freestanding L0 is not the full Ninja executable.
- No performance improvement is claimed without measurement against a declared baseline.
- Stacklessness, branch count, instruction count, and tail-call absence are codegen properties and must be verified per compiler/target/options.
- The optional x86-64 assembly does not establish equivalent behavior on ARM, RISC-V, POWER, s390x, or other ISAs.
