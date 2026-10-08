# RAFCODE command model

Copyright 2026 Rafael Melo Reis  

This is an authored requirements model, not a translation of another build tool.

The core models operation classes without performing operating-system calls.

## Classes

- **CONTROL** — VOID, BOOT, RESET, HALT
- **OBJECT** — DECLARE, RELEASE
- **RELATION** — RELATE
- **EXECUTION** — ARM, COMMIT, CANCEL
- **OBSERVATION** — OBSERVE, ACK

A later adapter may map these classes to filesystem, process, device, network, scheduler,
or build-worker capabilities. Such adapters are outside L0 and require their own rights,
runtime and evidence gates.

## Representation

One immutable command cell is 16 bytes:

`op:u8 | flags:u8 | lane:u16 | subject:u32 | left:u32 | right:u32`

The L0 protocol has no command-line grammar, token parser, strings, paths, shell,
environment variables, filesystem semantics, job graph format, or operating-system ABI.

The design goal is to minimize hidden state and abstraction by making each transition
explicit and caller-owned.
