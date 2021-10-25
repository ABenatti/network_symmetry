# Network symmetry

Fast library, written in C for python to calculate network Accessibility and Symmetry. More information regarding these measurements are described in the papers listed as follows:

[Travençolo, Bruno Augusto Nassif, and L. da F. Costa. "Accessibility in complex networks." Physics Letters A 373, no. 1 (2008): 89-95.](https://doi.org/10.1016/j.physleta.2008.10.069)

[Silva, Filipi N., Cesar H. Comin, Thomas K. DM Peron, Francisco A. Rodrigues, Cheng Ye, Richard C. Wilson, Edwin R. Hancock, and Luciano da F. Costa. "Concentric network symmetry." Information Sciences 333 (2016): 61-80.](https://arxiv.org/abs/1407.0224)

If you use this code in a scientific study, please cite the respective references and this library.

A comprehensive guide to the theory and applications of the accessibility measurements is available from: [Benatti, Alexandre, and Luciano da F. Costa. "Accessibility: Generalizing the Node Degree (A Tutorial)." (2021).](https://www.researchgate.net/publication/355081440_Accessibility_Generalizing_the_Node_Degree_CDT-62)

## Install

Requires python headers and a C11 compatible compiler, such as gcc or clang.

To install it, simply run:

```bash
pip install network-symmetry
```

or clone this repository and install it from master by running:

```bash
pip install git+https://github.com/ABenatti/network_symmetry.git
```
## Usage
Step 1: Import the libraries

```python
import numpy as np
import network_symmetry as ns
```

Step 2: Convert network to an edge list and a list of weights (optional)

```python
vertex_count = 10
edges = np.array([(0, 1), (0, 2), (1, 2), (0, 3), (1, 3), (2, 3), (2, 4), (3, 4), (0, 4),
                  (4, 5), (3, 5), (1, 5), (1, 6), (3, 6), (4, 6), (5, 7), (4, 7), (0, 7), 
                  (5, 8), (4, 8), (3, 8), (3, 9), (7, 9), (0, 9)])
weights = np.random.random(size=edges.shape[0])
directed = False
```

Step 3: Load the network data in a measurer object

```python
measurer = ns.Network(vertex_count = vertex_count, 
                      edges = edges, 
                      directed = directed, 
                      weights = weights
                      )
```

Step 4: Set the parameters:
```python
h_max = 3
measurer.set_parameters(h_max= h_max)
```

Step 5: Calculate the measurements: 
```python
measurer.compute_symmetry()
```

Step 6: The outputs can be seen as follows.
```python
print("\nResults:")
for h in range(2,h_max+1):
    print("h =", h)
    print(" Accessibility:")
    print(" ", measurer.accessibility(h))
    print(" Symmetry (backbone):")
    print(" ",measurer.symmetry_backbone(h))
    print(" Symmetry (merged):")
    print(" ",measurer.symmetry_merged(h))
```
**Important:** In order to be faster, this version of accessibility considers a random walk in which the walker cannot return to the already visited nodes.

## API Documentation
```python
measurer = ns.Network(vertex_count = vertex_count, 
                      edges = edges, 
                      directed = directed, 
                      weights= weights
                      )
```
- `vertex_count` - number of vertices in the network;
- `edges` - list of edges;
- `directed` - directed or not;
- `weights` - list containing the weights of the edges (use the same order as edges).


```python
measurer.set_parameters(h_max = 2,
                        merge_last_level = True,
                        live_stream = False,
                        parallel_jobs = 1,
                        verbose = False,
                        show_status = True
                        )
```
- `h_max` - Compute all symmetries and accessibilities for h=2 to h_max, which must be greater or equal to 2;
- `merge_last_level` - Merge the last level. True by default;
- `live_stream` - Stream the output as results are obtained. Note that the results may be out of order;
- `parallel_jobs` - The number of parallel jobs, which must be greater or equal to 1; 
- `verbose` - If True, shows the calculation steps;
- `show_status` - If True, show the progress of the calculation.


```python
measurer.compute_symmetry()
```
Compute symmetries and accessibilities by using the parameters set in "set_parameters".


```python
accessibility = measurer.accessibility(h)
symmetry_backbone = measurer.symmetry_backbone(h)
symmetry_merged = measurer.symmetry_merged(h)
```
- `h`- desired number of steps.
These methods return the respective lists measurements. The order of measures in the lists follows the node orders.

## Libraries
All of these codes were developed and executed with the environment described in "requirements.txt". 

## Citation Request
If you publish a paper related to this material, please cite this repository and the respective papers.

## Acknowledgements
Alexandre Benatti thanks Coordenação de Aperfeiçoamento de Pessoal de Nível Superior – Brasil (CAPES) – Finance Code (001) (grant no. 88882.328749/2019-01). Henrique F. de Arruda acknowledges FAPESP for sponsorship (grant no. 2018/10489-0). Luciano da F. Costa thanks CNPq (grant no. 307085/2018-0) and NAP-PRP-USP for sponsorship. This work has been supported also by FAPESP grant no. 2015/22308-2.

## License
This software is under the following license.

```
Copyright (c) 2021 network-accessibility

network-accessibility (c) by Alexandre Benatti, Henrique Ferraz de Arruda
Filipi Nascimento Silva, and Luciano da Fontoura Costa

network-accessibility is licensed under a
Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.

You should have received a copy of the license along with this
work. If not, see <http://creativecommons.org/licenses/by-nc-sa/4.0/>. 

Software provided as is and with absolutely no warranty, express or implied, 
with no liability for claim or damage.
```
