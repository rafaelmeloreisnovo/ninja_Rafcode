# Authorship, provenance and clean-room boundary

Author of the files in this snapshot: **Rafael Melo Reis, 2026**.

The Git repository has earlier history derived from the Ninja project. That historical
material is not relabelled, erased, or claimed as RAFCODE authorship.

The current snapshot is a new tree and does not intentionally carry source code,
file structure, parser/build-graph organization, internal identifiers, command syntax,
or implementation logic from Ninja.

## Important limitation

The implementer in the present session had already inspected portions of upstream Ninja
source before this reset. Therefore this work must **not** be represented as a formally
certified clean-room implementation.

Allowed claim:

> independent authored rewrite from generic functional requirements, with no intentional
> source/structure copying in this replacement snapshot.

A stricter clean-room claim would require organizational separation between specification
extraction and implementation, with the implementer denied access to the original source.

## P0 rights gate

Before any third-party material is incorporated:

`SOURCE -> RIGHTSHOLDER -> LICENSE -> OBLIGATIONS -> COMPATIBILITY -> ATTRIBUTION -> EVIDENCE`

Unknown or ambiguous license identity is `TOKEN_VAZIO/BLOCKED`.

A license label such as "PET" is **not inferred** without an exact license text or stable
identifier. No material is accepted merely because it is public, forkable, or accessible.
