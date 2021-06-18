# CXRandomWalk

Fast library written in C for python to generate sentences based on random walks from networks. Can be used to generate input data for word2vec and other embedding techniques.

## Install

Requires python headers and a C11 compatible compiler, such as gcc or clang.

To install it, simply run:

```bash
pip install cxrandomwalk
```

or clone this repository and install it from master by running:

```bash
pip install git+git://github.com/filipinascimento/cxrandomwalk.git
```
## Usage
The walking algorithm uses regular random walks and biased random walks according to node2vec ([https://snap.stanford.edu/node2vec/]).

Step 1: Import the libraries

```python
import numpy as np
import cxrandomwalk as rw
```

Step 2: Convert network to a edgelist and a list of weights (optional)

```python
vertexCount = 10;
edges = np.random.randint(0,vertexCount-1,(vertexCount*2, 2))
weights = np.random.random(size=vertexCount*2);
```

Step 3: Load the network data in an agent object

```python
agent = rw.Agent(vertexCount,edges,False,weights)
```

To generate the sequences use: 
```python
sequences = agent.generateWalks(p=2,q=3,verbose=True)
```

The output is a list of lists with the walks.


## API Documentation
```python
cxrandomwalk.Agent(vertexCount,edges,directed=False,weights=None)
```
- `vertexCount` - number of vertices in the network
- `edges` - list of edges 
- `directed` - directed or not
- `weights` - list containing the weights of the edges (use the same order as edges)
~returns a rw.Agent instance~ 

```python
cxrandomwalk.Agent.generateWalks(p=1.0,q=1.0,windowSize=80,walksPerNode=10,verbose=False,filename=None,callback=None,updateInterval=1000)
```
- `p` - for p in node2vec walks (1.0 for unbiased) 
- `q` - for q in node2vec walks (1.0 for unbiased) 
- `windowSize` - length of each walk 
- `walksPerNode` - number of sentences to be generated 
- `verbose` - enable verbose mode
- `filename` - save sentences to filename instead of returning the sequences
- `callback` - callback function that runs 
- `updateInterval` - set the interval between iterations so that python interruptions work 

returns a list of lists containing the sequences. If filename is provided, instead of returning a list, the method creates the file and fill with the walks, with words separated by spaces and sentences separated by new lines. It can be used directly on gensim.

## TODO
- Include other kinds of walks
- Better documentation and CLI help


## Example with tqdm callback

```python
import numpy as np
import cxrandomwalk as rw
from tqdm.auto import tqdm 

vertexCount = 10000;
edges = np.random.randint(0,vertexCount-1,(vertexCount*2, 2))
weights = np.random.random(size=vertexCount*2);

agent = rw.Agent(vertexCount,edges,False,weights)

def make_pbar():
  pbar = None
  def inner(current,total):
    nonlocal pbar
    if(pbar is None):
      pbar= tqdm(total=total);
    pbar.update(current - pbar.n)
  return inner

print(len(agent.generateWalks(q=1.0,p=1.0,verbose=False,updateInterval=1000,callback=make_pbar())))

```


