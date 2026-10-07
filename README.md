# RAFCODE Origin

Copyright 2026 Rafael Melo Reis  
SPDX-License-Identifier: Apache-2.0

A new authored freestanding command nucleus.

This snapshot is intentionally **not organized as Ninja** and does not reuse Ninja
source files, parser model, build-graph implementation, command syntax, internal names,
or directory topology. It is designed from generic system-operation requirements only.

## L0 contract

`void rafk_step(const raf_frame*, raf_state*, raf_event*)`

The core consumes one fixed command cell and performs one bounded state transition:

- caller-owned input/state/event memory;
- no libc headers/calls;
- no heap;
- no syscalls;
- no recursion;
- no source-level loop in the L0 transition;
- no strings, shell, environment, path parser, filesystem or process API;
- no external runtime symbol required by the core object;
- platform adapters are a later boundary, never hidden inside L0.

Operation classes:

`CONTROL | OBJECT | RELATION | EXECUTION | OBSERVATION`.

Current opcodes:

`VOID | BOOT | DECLARE | RELATE | ARM | COMMIT | OBSERVE | ACK | CANCEL | RELEASE | RESET | HALT`.

These are authored protocol verbs. They are not wrappers for POSIX, Win32, Ninja,
or another command language.

## Evidence boundary

`SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM`

`IMPLEMENTED_UNTESTED != PASS`

`BUILD_PROVIDER != RUNTIME_DEPENDENCY`

`TOKEN_VAZIO != 0`

Local build/codegen evidence never implies physical-device or performance evidence.
