# NOVOexport incremental plan adapter (RAFCODE authorial boundary)

**Status:** `PLAN_ONLY`, no outbound license granted, no CI/device proof yet.

This is a **metadata-only, offline** Python 3 stdlib adapter inspired by Ninja's
incremental dependency planning. It is not Ninja itself, not a build executor,
not a freestanding binary, and does not modify `rafcode/command/`, `rafcode/l0/`,
or upstream `src/**`. Existing authorial nuclei remain independent.

The raw NOVOexport corpus stays in its original Drive custody. **No redirects,
network clients, synchronization, downloads, uploads, shell commands, file
mutation, private conversation text or credentials.** Input is an explicitly
provided *local metadata manifest*; output is JSON to stdout only. The tool
neither discovers the Drive folder nor claims remote coverage. It does not
persist or promote fingerprints on its own.

## One command

```sh
python3 rafcode/novoexport/planner.py /path/to/local-metadata.json
python3 -m unittest discover -s rafcode/novoexport -p 'test_*.py'
```

Example `local-metadata.json` (all IDs are **synthetic**):

```json
{"nodes": [
  {"id":"shard_045", "source_ref":"FAKE_DRIVE_ID_A", "version":"rev_1", "role":"USER", "deps":[]},
  {"id":"token_index_045", "source_ref":"FAKE_DERIVED_ID_B", "version":"schema_v1", "role":"DERIVED", "deps":["shard_045"]}
]}
```

Supply a separate, **independently verified** state as the optional second
argument: `{"fingerprints":{"shard_045":"<64 lowercase hex>",...}}`.
`candidate_fingerprints` in output are **proposed**, not proof that outputs
were actually built, written, reviewed or checked. Promote them to a future
previous state **only** after the corresponding work completes and an external
receipt, exact revision and human/provider gates verify the result. No
automatic commit or caching occurs.

- `REBUILD`: missing or different last verified fingerprint, including changes
  inherited through the dependency DAG.
- `SKIP`: identical to a previously **verified** input fingerprint; not proof
  that a remote artifact still exists.
- `BLOCKED`: missing dependency, `TOKEN_VAZIO` source/version, or transitively
  blocked upstream. Dependency cycles, duplicate IDs, unknown fields, invalid
  IDs, URLs, embedded secrets, and invalid prior-state digests fail closed.

Only opaque alphanumeric/underscore/hyphen identifiers are permitted. Role is
explicit (`USER`, `ASSISTANT`, `DERIVED`) and included in the fingerprint so
assistant responses cannot masquerade as user inputs. Source bytes, SHA of
content, completion, licensing and authenticity are **not** proven here.

## External custody / constraints

The owner-managed NOVOexport Drive root and its `00_INDEX` / `02_ROUTES` /
`08_RECEIPTS` directories remain authoritative for corpus custody, routing,
and receipts. Do not mirror raw shards into this public repository. Mapping
from real Drive IDs and version proofs belongs to a separate authorized local
manifest, not this tracked source tree.

- `TMD_SCOPE=rafcode/novoexport`
- `TMD_SOURCE=opaque_metadata_only`
- `TMD_DELTA=deterministic_incremental_plan`
- `TMD_EVIDENCE=local_planner_logic_only`
- `TMD_STATE=TOKEN_VAZIO` (expansion/semantics are unresolved)
- `OUTBOUND_LICENSE_GRANT=false` (no license inferred)
- `claim_allowed=false` until independent exact-head CI and runtime gates.

Rollback: close the proposal without merge. If later merged, revert the
adapter-only commit; do not alter upstream Ninja or Drive raw originals.
