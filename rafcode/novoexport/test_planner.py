"""Synthetic, offline falsifiers for metadata-only NOVOexport planning."""
import copy
import unittest
from planner import InvalidPlan, TOKEN_VAZIO, plan


def node(name, ref="DRIVE_123", version="v1", role="USER", deps=None):
    return {"id": name, "source_ref": ref, "version": version,
            "role": role, "deps": [] if deps is None else deps}


class TestPlanner(unittest.TestCase):
    def test_first_run_rebuilds(self):
        out = plan({"nodes": [node("a")]})
        self.assertEqual(out["jobs"][0]["action"], "REBUILD")
        self.assertEqual(out["mode"], "PLAN_ONLY")
        self.assertFalse(out["claim_allowed"])
        self.assertTrue(out["no_redirect"])

    def test_second_run_skips(self):
        m = {"nodes": [node("a")]}
        first = plan(m)
        self.assertEqual(plan(m, {"fingerprints": first["candidate_fingerprints"]})["jobs"][0]["action"], "SKIP")

    def test_changed_version_rebuilds_dependents_only(self):
        m = {"nodes": [node("a"), node("b", role="DERIVED", deps=["a"]), node("c")]}
        p = {"fingerprints": plan(m)["candidate_fingerprints"]}
        m["nodes"][0]["version"] = "v2"
        out = plan(m, p)
        self.assertEqual({j["id"]: j["action"] for j in out["jobs"]},
                         {"a": "REBUILD", "b": "REBUILD", "c": "SKIP"})

    def test_unknown_source_blocks_dependent(self):
        out = plan({"nodes": [node("a", ref=TOKEN_VAZIO), node("b", role="DERIVED", deps=["a"])]})
        self.assertEqual([j["action"] for j in out["jobs"]], ["BLOCKED", "BLOCKED"])
        self.assertEqual(out["candidate_fingerprints"], {})

    def test_unknown_version_not_zero(self):
        out = plan({"nodes": [node("a", version=TOKEN_VAZIO)]})
        self.assertEqual(out["jobs"][0]["action"], "BLOCKED")
        with self.assertRaises(InvalidPlan):
            plan({"nodes": [node("a", version=0)]})

    def test_missing_dependency_blocks(self):
        out = plan({"nodes": [node("a", deps=["missing"])]})
        self.assertEqual(out["jobs"][0]["action"], "BLOCKED")

    def test_cycle_rejected(self):
        with self.assertRaisesRegex(InvalidPlan, "cycle"):
            plan({"nodes": [node("a", deps=["b"]), node("b", deps=["a"])]})

    def test_duplicate_nodes_rejected(self):
        with self.assertRaises(InvalidPlan):
            plan({"nodes": [node("a"), node("a")]})

    def test_no_urls_or_secret_strings(self):
        for bad in ("https://drive.google.com/file", "../outside", "secret@host", "id?token=x", "foo/bar"):
            with self.subTest(bad=bad), self.assertRaises(InvalidPlan):
                plan({"nodes": [node("a", ref=bad)]})

    def test_role_separation_and_fingerprint(self):
        m = {"nodes": [node("a", role="USER")]}
        x = plan(m)
        m["nodes"][0]["role"] = "ASSISTANT"
        self.assertEqual(plan(m, {"fingerprints": x["candidate_fingerprints"]})["jobs"][0]["action"], "REBUILD")

    def test_input_order_deterministic(self):
        m = {"nodes": [node("z"), node("a"), node("b", role="DERIVED", deps=["z", "a"])]}
        swapped = copy.deepcopy(m)
        swapped["nodes"].reverse()
        swapped["nodes"][0]["deps"].reverse()
        self.assertEqual(plan(m), plan(swapped))

    def test_unhashable_role_fails_closed(self):
        m = {"nodes": [node("a", role=[])]}
        with self.assertRaises(InvalidPlan):
            plan(m)

    def test_reject_unsanctioned_schema(self):
        with self.assertRaises(InvalidPlan):
            plan({"nodes": [], "payload": "raw"})
        with self.assertRaises(InvalidPlan):
            plan({"nodes": [dict(node("a"), text="private")]})

    def test_invalid_prior_fingerprint(self):
        with self.assertRaises(InvalidPlan):
            plan({"nodes": [node("a")]}, {"fingerprints": {"a": "PASS"}})

    def test_empty_manifest_is_not_source_proof(self):
        out = plan({"nodes": []})
        self.assertEqual(out["jobs"], [])
        self.assertFalse(out["claim_allowed"])


if __name__ == "__main__":
    unittest.main()
