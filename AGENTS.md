# AGENTS.md — ninja_Rafcode

STATUS: ACTIVE
OWNER: Rafael Melo Reis
DATE_LOCAL: 2026-10-07

## 0. Authority and boundaries

This repository contains two independent boundaries:

- UPSTREAM_NINJA: the inherited Ninja tree.
- RAFCODE_AUTHORIAL: `rafcode/**` only.

Canonical upstream baseline for this authorial split:
`ninja-build/ninja@4e4df1e567eb3c1475a51af261cba2bfff60b4be`.

Invariant:
`UPSTREAM_NINJA != RAFCODE_AUTHORIAL`.

A RAFCODE task MUST NOT remove, rename, relabel, relicense, or rewrite an
upstream Ninja file unless the owner explicitly requests an upstream change.

## 1. Authorial nuclei

The RAFCODE boundary has two separate nuclei. They are complementary and MUST
remain distinguishable.

### command nucleus
Root: `rafcode/command/`

- `src/rafk_core.c`
- `include/rafk_protocol.h`
- `include/rafk_types.h`
- `arch/x86_64/rafk_hint.S`
- `link/rafk.ld`
- `tests/semantic.c`

Role: fixed-frame command/state transition nucleus.

### l0 nucleus
Root: `rafcode/l0/`

- `src/raf_l0.c`
- `include/raf_l0.h`
- `arch/x86_64/raf_leaf.S`
- `link/raf_l0.ld`
- `tests/semantic.c`

Role: explicit absent/empty/NUL/space/byte/sequence observation nucleus.

Do not collapse these nuclei into one API merely because both are
freestanding-oriented.

## 2. Rights P0

The upstream `COPYING` belongs to the inherited Ninja boundary.

For `rafcode/**`:
- authorship/copyright and outbound license are separate questions;
- do not infer an outbound license from the upstream Ninja license;
- do not add an SPDX license identifier without explicit owner authorization;
- `FORK != COPYRIGHT_TRANSFER`;
- `UPSTREAM_LICENSE != RAFCODE_LICENSE`.

If an outbound RAFCODE license is not explicitly resolved:
`LICENSE_STATE=TOKEN_VAZIO` and `OUTBOUND_LICENSE_GRANT=false`.

## 3. TMD gate

TMD is required by the owner, but its exact expansion/semantics are not
canonically defined in this repository.

Therefore:
- `TMD_REQUIRED=true`
- `TMD_EXPANSION=TOKEN_VAZIO`
- `TMD_STATE=TOKEN_VAZIO`

Do NOT guess or silently expand TMD.

Until the owner defines the exact TMD contract, every RAFCODE change record
must at minimum preserve these literal fields:

- `TMD_SCOPE`
- `TMD_SOURCE`
- `TMD_DELTA`
- `TMD_EVIDENCE`
- `TMD_STATE`

If an operation depends on a semantic meaning of TMD that is not explicitly
defined, that operation is blocked rather than guessed.

## 4. Freestanding claim boundary

For RAFCODE core claims, distinguish source, object, linked artifact,
execution, evidence, and claim.

Invariant:
`SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM`.

Also:
- `TOKEN_VAZIO != 0`
- `IMPLEMENTED_UNTESTED != PASS`

Do not claim whole-binary freestanding from source inspection alone.
Build tools/compilers/linkers are provider/tooling dependencies and are not
automatically runtime dependencies.

A source may be called freestanding-oriented only within the properties
actually verified for that source. Runtime/ELF/physical claims require their
own evidence.

## 5. Promotion gate

Before promoting a RAFCODE change toward `master`:

1. compare against the canonical upstream baseline;
2. upstream files removed by the RAFCODE delta MUST equal 0;
3. upstream files modified by the RAFCODE delta MUST equal 0 unless the owner
   explicitly requested an upstream change;
4. authorial implementation changes must stay under `rafcode/**`;
5. rights state must not be inferred;
6. exact-head CI/runtime evidence is PASS only when actually observed.

Historical PRs #1 through #5 and their destructive rewrite-derived branches are
evidence/history only and MUST NOT be used as promotion bases.

## 6. Operational close

Every material operation closes with:

`R3=<F_ok,F_gap,F_next>`

Prefer preservation, provenance, exact-path reconstruction, and reversible
changes over deletion or silent rewriting.
