import numpy as np
import network_symmetry as ns

vertex_count = 10
edges = np.array([(0, 1), (0, 2), (1, 2), (0, 3), (1, 3), (2, 3), (2, 4), (3, 4), (0, 4), (4, 5), (3, 5), (1, 5), (1, 6), (3, 6), (4, 6), (5, 7), (4, 7), (0, 7), (5, 8), (4, 8), (3, 8), (3, 9), (7, 9), (0, 9)])
weights = np.random.random(size=edges.shape[0])

directed = False
measurer = ns.Network(vertex_count = vertex_count, edges = edges, directed = directed, weights= weights)
h_max = 3
measurer.set_parameters(h_max= h_max)
measurer.compute_symmetry()

print("Show summary:")
measurer.show_summary()

print("\nResults:")
for h in range(2,h_max+1):
    print("h =", h)
    print(" Accessibility:")
    print(" ", measurer.accessibility(h))
    print(" Symmetry (backbone):")
    print(" ",measurer.symmetry_backbone(h))
    print(" Symmetry (merged):")
    print(" ",measurer.symmetry_merged(h))
