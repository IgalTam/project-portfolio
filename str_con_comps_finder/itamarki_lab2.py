import sys

class node:

	def __init__(self,name,out_edges=[],in_edges=[],previsit=-1, postvisit=-1,component = None):
		self.name: int = name
		self.out_edges = out_edges
		self.in_edges = in_edges
		self.previsit = previsit
		self.postvisit = postvisit
		self.component = component

class Queue:
    """queue class to support strong_connectivity()
    (treated more like a stack due to the reversal of out edges in 2nd pass)
    """

    def __init__(self):
        """constructor"""
        self.q_arr: list = []
        self.q_len: int = 0

    def enqueue(self, item):
        """inserts item into queue"""
        self.q_arr.append(item)
        self.q_len += 1
    
    def dequeue(self):
        """removes and returns first element in queue"""
        if self.q_len == 0:
            return None
        first = self.q_arr[0]
        self.q_arr = self.q_arr[1:]
        self.q_len -= 1
        return first

def strong_connectivity(G: list):
    """
    input: a directed graph, G
    output: a list of all connected components in G, sorted by ascending order of ID
    """
    # variable declarations
    components: list = []
    q = Queue()
    visit_count: int = 0

    for vertex in G:    # dfs first pass
        visit_count = explore(vertex.name, q, G, visit_count)

    vertex = q.dequeue() # second pass
    while vertex is not None:
        temp_list = assign(vertex.name, vertex.name, G, visit_count, [])
        if temp_list:
            components.append(temp_list)
        vertex = q.dequeue()

    # sort components
    sort_component_list(components)

    return components

def explore(vertex: int, q: Queue, graph: list[node], visit_count: int):
    """recursive dfs helper"""
    if graph[vertex].previsit == -1:   # vertex unvisited
        visit_count += 1    # record previsit
        graph[vertex].previsit = visit_count
        for neighbor in graph[vertex].out_edges:    # explore neighboring nodes
            visit_count = explore(graph[neighbor].name, q, graph, visit_count)
        visit_count += 1    # record postvisit
        graph[vertex].postvisit = visit_count
        q.enqueue(graph[vertex])        # enqueue after post visit
    return visit_count

def assign(vertex: int, root: int, graph: list[node], visit_count: int, ret_list: list):
    """recursively assigns vertices to components"""
    if graph[vertex].component is None:
        graph[vertex].component = root # vertex becomes member of the component designated by root
        ret_list.append(vertex) # append to list for specific component
        for neighbor in graph[vertex].out_edges:    # repeat for neighbors after reversing out edges
            assign(graph[neighbor].name, root, graph, visit_count, ret_list)
    return ret_list

def sort_component_list(components):
	for c in components:
		c.sort()
	components.sort(key = lambda x: x[0])

def read_file(filename):
	with open(filename) as f:
		lines = f.readlines()
		v = int(lines[0])
		if  v == 0:
			raise ValueError("Graph must have one or more vertices")
		G = list(node(name = i, in_edges=[],out_edges=[],previsit= -1, postvisit=-1, component=None) for i in range(v))
		for l in lines[1:]:
			tokens = l.split(",")
			fromVertex,toVertex = (int(tokens[0]),int(tokens[1]))
			G[fromVertex].out_edges.append(toVertex)
			G[toVertex].in_edges.append(fromVertex)
		return G


def main():
	filename = sys.argv[1]
	G = read_file(filename)
	components = strong_connectivity(G)
	print(components)

if __name__ == "__main__":
    main()
