import numpy as np
import network_symmetry as ns
# from tqdm.auto import tqdm 

vertexCount = 10
edges = np.random.randint(0,vertexCount,(vertexCount*4, 2))
weights = np.random.random(size=vertexCount*4);
names = ["ID %d"%i for i in range(vertexCount)]

measurer = ns.Measurer(vertexCount,edges,False,weights)

# network = "/Users/henriqueferrazdearruda/Downloads/Symmetry/network_symmetry/Python/mechanics.xnet"
l = measurer._compute("-c", "-M", "-l", "3", "-a", "", "", "", "", "", "", "", '', "output.tsv")

n_measures = len(l)//3
out = {"Accessibility": dict(), "Symmetry Backbone": dict(), "Symmetry Merged": dict()}
pos = 0
for i in range(n_measures):
    out["Accessibility"][i+2] = l[i + pos]
    out["Symmetry Backbone"][i+2] = l[i + 1 + pos]
    out["Symmetry Merged"][i+2] = l[i + 2 + pos]
    pos += 1

print (out)
# print(result)
