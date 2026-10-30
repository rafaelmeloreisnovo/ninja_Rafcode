"""Metadata-only incremental plan for NOVOexport. Python 3 stdlib; no network/writes.

This is an orchestration adapter, NOT a freestanding RAFCODE core.
"""
import hashlib
import json
import re
import sys

TOKEN_VAZIO = "TOKEN_VAZIO"
IDENT = re.compile(r"^[A-Za-z0-9_-]{1,128}$")
HEX = re.compile(r"^[0-9a-f]{64}$")
ROLES = frozenset(("USER", "ASSISTANT", "DERIVED"))


class InvalidPlan(ValueError):
    pass


def identifier(value, name, unknown=False):
    if unknown and value == TOKEN_VAZIO:
        return value
    if not isinstance(value, str) or not IDENT.fullmatch(value) or value == TOKEN_VAZIO:
        raise InvalidPlan("invalid " + name + " (opaque IDs only, never URLs or secrets)")
    return value


def load_nodes(manifest):
    if not isinstance(manifest, dict) or set(manifest) != {"nodes"}:
        raise InvalidPlan("manifest must contain only nodes")
    if not isinstance(manifest["nodes"], list) or len(manifest["nodes"]) > 10000:
        raise InvalidPlan("nodes must be a list of at most 10000 metadata entries")
    nodes = {}
    for raw in manifest["nodes"]:
        if not isinstance(raw, dict) or set(raw) != {"id", "source_ref", "version", "role", "deps"}:
            raise InvalidPlan("each node requires id/source_ref/version/role/deps only")
        name = identifier(raw["id"], "id")
        if name in nodes:
            raise InvalidPlan("duplicate node " + name)
        source = identifier(raw["source_ref"], "source_ref", unknown=True)
        version = identifier(raw["version"], "version", unknown=True)
        if not isinstance(raw["role"], str) or raw["role"] not in ROLES:
            raise InvalidPlan("invalid role for " + name)
        deps = raw["deps"]
        if not isinstance(deps, list) or len(deps) > 10000 or any(not isinstance(d, str) for d in deps):
            raise InvalidPlan("deps must be an array of identifiers")
        for dep in deps:
            identifier(dep, "dependency")
        if len(set(deps)) != len(deps) or name in deps:
            raise InvalidPlan("duplicate/self dependency for " + name)
        nodes[name] = {"id": name, "source_ref": source, "version": version,
                       "role": raw["role"], "deps": sorted(deps)}
    return nodes


def plan(manifest, previous=None):
    nodes = load_nodes(manifest)
    if previous is None:
        previous = {"fingerprints": {}}
    if not isinstance(previous, dict) or set(previous) != {"fingerprints"} or not isinstance(previous["fingerprints"], dict):
        raise InvalidPlan("previous state must contain only fingerprints")
    for name, digest in previous["fingerprints"].items():
        identifier(name, "previous id")
        if not isinstance(digest, str) or not HEX.fullmatch(digest):
            raise InvalidPlan("invalid previous fingerprint")
    ordered, visited, active = [], set(), set()

    def visit(name):
        if name in active:
            raise InvalidPlan("dependency cycle at " + name)
        if name in visited:
            return
        active.add(name)
        for dep in nodes[name]["deps"]:
            if dep in nodes:
                visit(dep)
        active.remove(name)
        visited.add(name)
        ordered.append(name)

    for name in sorted(nodes):
        visit(name)
    states = {}
    digests = {}
    jobs = []
    for name in ordered:
        n = nodes[name]
        missing = [d for d in n["deps"] if d not in nodes]
        blocked_deps = [d for d in n["deps"] if states.get(d) == "BLOCKED"]
        if n["source_ref"] == TOKEN_VAZIO or n["version"] == TOKEN_VAZIO or missing or blocked_deps:
            status, digest = "BLOCKED", None
        else:
            canonical = json.dumps({"node": n, "parents": {d: digests[d] for d in n["deps"]}},
                                   ensure_ascii=True, sort_keys=True, separators=(",", ":"))
            digest = hashlib.sha256(canonical.encode("utf-8")).hexdigest()
            status = "SKIP" if previous["fingerprints"].get(name) == digest else "REBUILD"
            digests[name] = digest
        states[name] = status
        jobs.append({"id": name, "action": status, "fingerprint": digest,
                     "gap": ("missing_source_or_version_or_dependency" if status == "BLOCKED" else None)})
    return {"schema": "rafcode.novoexport.plan.v1", "mode": "PLAN_ONLY",
            "claim_allowed": False, "no_redirect": True, "jobs": jobs,
            "candidate_fingerprints": digests,
            "TMD_SCOPE": "rafcode/novoexport", "TMD_SOURCE": "opaque_metadata_only",
            "TMD_DELTA": "deterministic_incremental_plan",
            "TMD_EVIDENCE": "local_planner_logic_only", "TMD_STATE": TOKEN_VAZIO}


def main(argv):
    if len(argv) not in (2, 3):
        raise InvalidPlan("usage: python3 planner.py manifest.json [previous.json]")
    with open(argv[1], "r", encoding="utf-8") as stream:
        manifest = json.load(stream)
    previous = None
    if len(argv) == 3:
        with open(argv[2], "r", encoding="utf-8") as stream:
            previous = json.load(stream)
    print(json.dumps(plan(manifest, previous), indent=2, sort_keys=True))


if __name__ == "__main__":
    try:
        main(sys.argv)
    except (InvalidPlan, ValueError, OSError) as exc:
        print("BLOCKED: " + str(exc), file=sys.stderr)
        sys.exit(2)
