"""Regression checks against the original NumPy 1 implementation.

Run against an installed package with:
    python -m unittest discover -s tests -v

The baseline is an independent capture, not a recomputation of expected metrics.
C metric comparisons are exact. Generalized accessibility uses the approved
relative and absolute tolerances of 1e-14 for dependency-dependent roundoff.
"""

import contextlib
import io
import json
from pathlib import Path
import unittest

import numpy as np
import network_symmetry as ns
from network_symmetry_core import Measurer


BASELINE = json.loads(
    (Path(__file__).parent / "data" / "metrics_baseline.json").read_text()
)


def make_network(case, representation="int64", parallel_jobs=1):
    edges = np.array(case["edges"], dtype=np.int64)
    weights = case["weights"]
    if representation == "list":
        edges = edges.tolist()
    elif representation == "int32":
        edges = edges.astype(np.int32)
    elif representation == "strided":
        buffer = np.zeros((len(edges), 4), dtype=np.int64)
        buffer[:, ::2] = edges
        edges = buffer[:, ::2]
        if weights is not None:
            buffer = np.zeros(len(weights) * 2, dtype=np.float64)
            buffer[::2] = weights
            weights = buffer[::2]
    network = ns.Network(case["vertex_count"], edges, case["directed"], weights)
    network.set_parameters(
        h_max=case["h_max"],
        merge_last_level=case["merge_last_level"],
        parallel_jobs=parallel_jobs,
        show_status=False,
    )
    return network


class MetricRegressionTests(unittest.TestCase):
    def assert_metrics(self, network, case):
        self.assertIsNone(network.compute_symmetry())
        for name in ("accessibility", "symmetry_backbone", "symmetry_merged"):
            for h in range(2, case["h_max"] + 1):
                with self.subTest(metric=name, h=h):
                    actual = getattr(network, name)(h)
                    self.assertIsInstance(actual, list)
                    self.assertEqual(len(actual), case["vertex_count"])
                    self.assertTrue(all(isinstance(value, float) for value in actual))
                    np.testing.assert_array_equal(actual, case["expected"][name][str(h)])

    def test_concentric_metrics_match_original(self):
        for case in BASELINE["cases"]:
            for representation in ("list", "int32", "int64", "strided"):
                with self.subTest(case=case["name"], representation=representation):
                    self.assert_metrics(make_network(case, representation), case)

    def test_generalized_accessibility_matches_original(self):
        for case in BASELINE["cases"]:
            with self.subTest(case=case["name"]):
                actual = make_network(case).accessibility_generalized()
                self.assertIsInstance(actual, np.ndarray)
                self.assertEqual(actual.dtype, np.dtype(np.float64))
                np.testing.assert_allclose(
                    actual, case["expected"]["accessibility_generalized"],
                    rtol=1e-14, atol=1e-14,
                )

    def test_repeated_computation_and_parallel_parameter(self):
        for case in BASELINE["cases"]:
            with self.subTest(case=case["name"]):
                network = make_network(case, parallel_jobs=2)
                self.assert_metrics(network, case)
                self.assert_metrics(network, case)

    def test_network_replacement_and_clear(self):
        first, second = BASELINE["cases"][:2]
        network = make_network(first)
        self.assert_metrics(network, first)
        edges = np.array(second["edges"], dtype=np.int64)
        network.set_network(second["vertex_count"], edges, second["directed"], second["weights"])
        self.assertIs(network.get_network()["edges"], edges)
        self.assert_metrics(network, second)
        network.clear_network()
        self.assertEqual(network.get_network(), {"vertex_count": 0, "edges": [], "weights": []})
        self.assertEqual(network.calculated_symmetries, {})

    def test_parameter_defaults_and_clamping(self):
        network = ns.Network()
        self.assertEqual(network.get_parameters(), {
            "h-max": 2, "merge-last-level": True, "live-stream": False,
            "parallel-jobs": 1, "verbose": False, "show-status": True,
        })
        with contextlib.redirect_stdout(io.StringIO()):
            network.set_parameters(h_max=0, parallel_jobs=0)
        self.assertEqual(network.get_parameters()["h-max"], 2)
        self.assertEqual(network.get_parameters()["parallel-jobs"], 1)

    def test_existing_input_errors(self):
        with self.assertRaisesRegex(TypeError, "vertexCount"):
            Measurer(0, [(0, 1)])
        with self.assertRaisesRegex(TypeError, "Weights should have the same dimension"):
            Measurer(3, [(0, 1), (1, 2)], False, [1.0])
        with self.assertRaisesRegex(TypeError, "Edge indices should not be higher"):
            Measurer(3, [(0, 3)])


if __name__ == "__main__":
    unittest.main()
