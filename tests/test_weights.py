"""Weighted metric checks with expectations derived from transition probabilities."""

import math
import unittest

import numpy as np
import network_symmetry as ns


METRICS = ("accessibility", "symmetry_backbone", "symmetry_merged")

# Deliberately put the edges out of vertex order: weights belong to edge rows,
# not to neighbor vertex numbers or positions in an adjacency list.
TREE_EDGES = [(2, 5), (0, 2), (1, 4), (0, 1), (1, 3)]
TREE_WEIGHTS = [5.0, 3.0, 6.0, 1.0, 2.0]

# From vertex 0, vertices 1 and 2 merge in shell 1, and vertices 4 and 5
# optionally merge in shell 2. Several original edges connect the same groups.
MERGING_EDGES = [
    (2, 6), (0, 2), (4, 5), (1, 4), (0, 3),
    (1, 2), (2, 4), (0, 1), (3, 6), (1, 5),
]
MERGING_WEIGHTS = [6.0, 3.0, 64.0, 1.0, 4.0, 32.0, 2.0, 1.0, 8.0, 3.0]


def effective_count(probabilities):
    """Exponential Shannon entropy for the supplied exact probabilities."""
    return math.exp(-math.fsum(p * math.log(p) for p in probabilities))


def weight_input(weights, representation):
    if representation == "list":
        return list(weights)
    if representation == "strided":
        buffer = np.full(2 * len(weights), -999.0, dtype=np.float64)
        buffer[::2] = weights
        return buffer[::2]
    return np.asarray(weights, dtype=representation)


def compute_network(vertex_count, edges, weights, directed=False,
                    merge_last_level=True, h_max=2):
    network = ns.Network(vertex_count, edges, directed, weights)
    network.set_parameters(
        h_max=h_max, merge_last_level=merge_last_level, show_status=False,
    )
    network.compute_symmetry()
    return network


class WeightedMetricTests(unittest.TestCase):
    def assert_close(self, actual, expected):
        # Probabilities and returned C metrics use single-precision CVFloat.
        np.testing.assert_allclose(actual, expected, rtol=5e-7, atol=1e-7)

    def test_weighted_tree_matches_hand_calculated_probabilities(self):
        # At 0, P(1)=1/4 and P(2)=3/4. At 1, P(3|1)=1/4 and
        # P(4|1)=3/4; vertex 2 has only vertex 5 as an outward neighbor.
        # Thus shell 2 has probabilities (1/16, 3/16, 3/4).
        accessibility = effective_count([1 / 16, 3 / 16, 3 / 4])
        for directed in (False, True):
            for merge_last_level in (False, True):
                for representation in ("list", "float32", "float64", "strided"):
                    with self.subTest(
                        directed=directed, merge_last_level=merge_last_level,
                        representation=representation,
                    ):
                        network = compute_network(
                            6, TREE_EDGES, weight_input(TREE_WEIGHTS, representation),
                            directed=directed, merge_last_level=merge_last_level,
                        )
                        self.assert_close(network.accessibility(2)[0], accessibility)
                        self.assert_close(network.symmetry_backbone(2)[0], accessibility / 3)
                        self.assert_close(network.symmetry_merged(2)[0], accessibility / 3)

    def test_merged_groups_preserve_each_original_edge_weight(self):
        # Backbone shell-1 probabilities are (1/8, 3/8, 1/2). Shell 2:
        # P(4)=1/8*1/4 + 3/8*1/4 = 1/8,
        # P(5)=1/8*3/4 = 3/32, P(6)=3/8*3/4 + 1/2 = 25/32.
        accessibility = effective_count([1 / 8, 3 / 32, 25 / 32])
        for merge_last_level in (False, True):
            with self.subTest(merge_last_level=merge_last_level):
                network = compute_network(
                    7, MERGING_EDGES, MERGING_WEIGHTS,
                    merge_last_level=merge_last_level,
                )
                self.assert_close(network.accessibility(2)[0], accessibility)
                self.assert_close(network.symmetry_backbone(2)[0], accessibility / 3)
                # Merging {1,2} gives mass 1/2 and outgoing weights 3,3,6
                # to vertices 4,5,6. Vertex 3 sends its remaining 1/2 to 6.
                probabilities = ([1 / 4, 3 / 4] if merge_last_level
                                 else [1 / 8, 1 / 8, 3 / 4])
                self.assert_close(
                    network.symmetry_merged(2)[0],
                    effective_count(probabilities) / len(probabilities),
                )

    def test_zero_weight_branch_does_not_poison_reachable_destinations(self):
        # Vertex 1 has zero total outgoing weight. Both its destinations are
        # also reached through vertex 2, with positive weights in a 1:3 ratio.
        # Conditional on reaching shell 2, their probabilities are 1/4, 3/4.
        edges = [(1, 3), (2, 4), (0, 1), (1, 4), (0, 2), (2, 3)]
        weights = [0.0, 3.0, 1.0, 0.0, 3.0, 1.0]
        accessibility = effective_count([1 / 4, 3 / 4])
        for directed in (False, True):
            for merge_last_level in (False, True):
                with self.subTest(directed=directed, merge_last_level=merge_last_level):
                    network = compute_network(
                        5, edges, weights, directed=directed,
                        merge_last_level=merge_last_level,
                    )
                    self.assert_close(network.accessibility(2)[0], accessibility)
                    self.assert_close(network.symmetry_backbone(2)[0], accessibility / 2)
                    self.assert_close(network.symmetry_merged(2)[0], accessibility / 2)

    def test_edge_order_does_not_change_weighted_metrics(self):
        cases = [
            (6, TREE_EDGES, TREE_WEIGHTS, True),
            (7, MERGING_EDGES, MERGING_WEIGHTS, False),
        ]
        for vertex_count, edges, weights, directed in cases:
            for merge_last_level in (False, True):
                expected = compute_network(
                    vertex_count, edges, weights, directed=directed,
                    merge_last_level=merge_last_level, h_max=3,
                )
                permutations = [
                    list(reversed(range(len(edges)))),
                    list(range(1, len(edges), 2)) + list(range(0, len(edges), 2)),
                ]
                for order in permutations:
                    with self.subTest(
                        directed=directed, merge_last_level=merge_last_level, order=order,
                    ):
                        actual = compute_network(
                            vertex_count, [edges[i] for i in order],
                            [weights[i] for i in order], directed=directed,
                            merge_last_level=merge_last_level, h_max=3,
                        )
                        for name in METRICS:
                            for h in (2, 3):
                                with self.subTest(metric=name, h=h):
                                    self.assert_close(
                                        getattr(actual, name)(h), getattr(expected, name)(h),
                                    )

    def test_unit_weights_exactly_match_unweighted_metrics(self):
        cases = [
            (6, TREE_EDGES, True),
            (6, TREE_EDGES, False),
            (7, MERGING_EDGES, False),
        ]
        for vertex_count, edges, directed in cases:
            for merge_last_level in (False, True):
                with self.subTest(
                    vertex_count=vertex_count, directed=directed,
                    merge_last_level=merge_last_level,
                ):
                    unweighted = compute_network(
                        vertex_count, edges, None, directed=directed,
                        merge_last_level=merge_last_level, h_max=3,
                    )
                    weighted = compute_network(
                        vertex_count, edges, np.ones(len(edges)), directed=directed,
                        merge_last_level=merge_last_level, h_max=3,
                    )
                    for name in METRICS:
                        for h in (2, 3):
                            with self.subTest(metric=name, h=h):
                                np.testing.assert_array_equal(
                                    getattr(weighted, name)(h), getattr(unweighted, name)(h),
                                )


if __name__ == "__main__":
    unittest.main()
