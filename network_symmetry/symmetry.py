from network_symmetry_core import Measurer
import numpy as np

class Network():
    """
    Class for calculating the concentric symmetry and accessibility of each network node.

    Parameters
    ----------
        vertex_count : integer, optional
                Number of network vertices.
                
        edges : numpy array, optional
            Array of the graph edges, with the shape of (# of edges, 2).

        directed: boolean, optional
            Undirected network (False) or directed network (True). False by default.
        
        weights: numpy array, optional
            Edge weights, with the length of the edge list. The default value is weights = None.
    """

    def __init__(self, vertex_count = None, edges = None, directed = False, weights = None):
        self.vertex_count = vertex_count
        self.edges = edges
        self.weights = weights
        self.directed = directed
        self.calculated_symmetries = dict()
        self.parameters = dict()
        self.set_parameters()

    def set_network(self, vertex_count, edges, directed, weights = None):
        """
        To set the network.

        Parameters
        ----------
            vertex_count : integer
                Number of network vertices.
                 
            edges : numpy array 
                Array of the graph edges, with the shape of (# of edges, 2).
            
            directed: boolean
                Undirected network (False) or directed network (True).

            weights: numpy array, optional
                Edge weights, with the length of the edge list. The default value is weights = None.
        """

        self.vertex_count = vertex_count
        self.edges = edges
        self.weights = weights
        self.directed = directed

    def get_network(self):
        """
        To get the network structure.
        
        Return
        ----------
        Dictionary {"vertex_count", "edges", "weights"} - network data.
        """
        return {"vertex_count": self.vertex_count, "edges": self.edges, "weights": self.weights}

    def clear_network(self):
        """
        Clear network data.
        """
        self.edges = []
        self.weights = []
        self.vertex_count = 0
        self.calculated_symmetries = dict()
       
    
    
    def set_parameters( self,
                        h_max = 2,
                        merge_last_level = True,
                        live_stream = False,
                        parallel_jobs = 1,
                        verbose = False,
                        show_status = True
                        ):
        """
        Set the parameters

        Parameters
        ----------
        h_max: integer
            Compute all symmetries and accessibilities for h=2 to h_max, which must be greater or equal to 2. The default value is h_max = 2.

        merge_last_level: boolean
            Merge the last level. True by default.

        live_stream: boolean
            Stream the output as results are obtained. Note that the results may be out of order. False by default.

        parallel_jobs: integer
            The number of parallel jobs, which must be greater or equal to 1. The default value is parallel_jobs = 1.

        verbose: boolean
            If True, shows the calculation steps. False by default.

        show_status: boolean
            If True, show the progress of the calculation. True by default.
        """
        
        if int(h_max) < 2:
            h_max = 2
            print("h_max must be greater or equal to 2!")
        self.parameters["h-max"] = int(h_max)
        self.parameters["merge-last-level"] = merge_last_level
        self.parameters["live-stream"] = live_stream
        if int(parallel_jobs) < 1:
            parallel_jobs = 1
            print("parallel_jobs must be greater or equal to 1!")
        self.parameters["parallel-jobs"] = int(parallel_jobs)
        self.parameters["verbose"] = verbose
        self.parameters["show-status"] = show_status

    def get_parameters(self):
        """
        To get the parameters.
        
        Return
        ----------
        Dictionary with all setted parameters.
        """
        return self.parameters

    def compute_symmetry(self):
        """
        This method computes the symmetries and accessibilities by considering all the parameters previously set (see set_parameters).
        """
        if np.shape(self.weights) == (): #no weights used
            measurer = Measurer(self.vertex_count,self.edges,self.directed)
        else:
            measurer = Measurer(self.vertex_count,self.edges,self.directed,self.weights)
        h_max = str(self.parameters['h-max'])
        m = ""
        if self.parameters["merge-last-level"]:
            m = "-M" 

        live_stream = ""
        if self.parameters["live-stream"]:
            live_stream = "-s"

        parallel_jobs = str(self.parameters["parallel-jobs"])

        verbose = ""
        if self.parameters["verbose"]:
            verbose = "-v"
        
        show_status = ""
        if self.parameters["show-status"]:
            show_status = "S"
        
        # print(show_status, m, "-l", h_max, live_stream, "-j", parallel_jobs, verbose, "-a")
        l = measurer._compute(show_status, m, "-l", h_max, live_stream, "-j", parallel_jobs, verbose, "-a")

        # n_measures = len(l)//3
        h_max = self.parameters['h-max']

        self.calculated_symmetries["Accessibility"] = dict()
        self.calculated_symmetries["Symmetry Backbone"] = dict()
        self.calculated_symmetries["Symmetry Merged"] = dict()

        for i,h in enumerate(range(2,h_max+1)):
            self.calculated_symmetries["Accessibility"][h] = l[i]
            self.calculated_symmetries["Symmetry Backbone"][h] = l[h_max-1+i]
            self.calculated_symmetries["Symmetry Merged"][h] = l[2*(h_max-1)+i]

    def show_summary(self):
        """
        Shows the summary of the calculated measurements.
        """
        for key in self.calculated_symmetries.keys():
            h_values = list(self.calculated_symmetries[key].keys())
            h_max = np.max(h_values)
            if h_max > 2:
                print(key, "for h between, 2 and", str(h_max))
            else:
                print(key, "for h = 2")

    def accessibility(self, h):
        """
        Return the pre-calculated accessibility for a given h, computed in compute_symmetry().

        Parameters
        ----------
        h: integer
            Desired number of steps.
        
        Return
        ----------
        Numpy array containing the accessibity of each network node.
        """
        key = "Accessibility"
        out = None
        if h in self.calculated_symmetries[key]:
            out = self.calculated_symmetries[key][h]
        else:
            print(key, "was not calculated for h =", h)
        return out


    def symmetry_backbone(self, h):
        """
        Return the pre-calculated Symmetry Backbone for a given h, computed in compute_symmetry().

        Parameters
        ----------
        h: integer
            Desired number of steps.
        
        Return
        ----------
        List containing the Symmetry Backbone of each network node.
        """
        key = "Symmetry Backbone"
        out = None
        if h in self.calculated_symmetries[key]:
            out = self.calculated_symmetries[key][h]
        else:
            print(key, "was not calculated for h =", h)
        return out

    def symmetry_merged(self, h):
        """
        Return the pre-calculated Symmetry Merged for a given h, computed in compute_symmetry().

        Parameters
        ----------
        h: integer
            Desired number of steps.
        
        Return
        ----------
        List containing the Symmetry Merged of each network node.
        """
        key = "Symmetry Merged"
        out = None
        if h in self.calculated_symmetries[key]:
            out = self.calculated_symmetries[key][h]
        else:
            print(key, "was not calculated for h =", h)
        return out
