# L0 evidence contract

States:

- `IMPLEMENTED_UNTESTED`: source exists, no execution evidence yet.
- `PASS_SCOPED_HOST`: declared host compile/semantic/audit gates passed for an exact source snapshot.
- `PASS_SCOPED_CODEGEN`: codegen properties were inspected for a declared compiler/target/options tuple.
- `TOKEN_VAZIO_PHYSICAL`: no physical-device execution receipt exists.

Required evidence before a codegen claim:

1. exact source commit/ref;
2. exact compiler and target tuple;
3. compile flags;
4. `nm -u` result;
5. disassembly evidence for stack/call/loop/tail properties being claimed;
6. ELF dynamic/interpreter readback for linked artifacts when applicable.

No later gate backfills an earlier missing gate.
