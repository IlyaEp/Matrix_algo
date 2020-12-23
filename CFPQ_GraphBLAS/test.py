import sys
import ctypes
import ctypes.util 

from time import time

lib = ctypes.CDLL('./libAllPaths.so')

LP_c_char = ctypes.POINTER(ctypes.c_char)

lib.grammar_new.argtypes = [LP_c_char]
lib.grammar_new.restype = ctypes.c_void_p

lib.grammar_del.argtypes = [ctypes.c_void_p]

lib.graph_new.argtypes = [LP_c_char]
lib.graph_new.restype = ctypes.c_void_p

lib.graph_del.argtypes = [ctypes.c_void_p]

lib.intersect.argtypes = [ctypes.c_void_p, ctypes.c_void_p]

lib.get_elements.argtypes = [ctypes.c_void_p, LP_c_char]
lib.get_elements.restype = ctypes.c_void_p

lib.string_del.argtypes = [ctypes.c_void_p]

lib.getpaths.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, LP_c_char, ctypes.c_int]
lib.getpaths.restype = ctypes.c_void_p

class MyAllPairsAllPaths:
	def __init__(self):
		lib.graphblas_init()
		self.mygrammar = None # <- index storage
		self.mygraph = None # <- graph storage
	
	def __del__(self):
		if self.mygrammar:
			lib.grammar_del(self.mygrammar)
		if self.mygraph:
			lib.graph_del(self.mygraph)
		lib.graphblas_finalize()

	def create_index(self, graph: str, grammar: str):
		if self.mygrammar:
			lib.grammar_del(self.mygrammar)
			self.mygrammar = None
		if self.mygraph:
			lib.graph_del(self.mygraph)
			self.mygraph = None
		self.mygrammar, self.mygraph = intersect(graph, grammar)

	def restore_paths(self, v, to, S, boundLen) -> [str]:
		return getpaths(self.mygrammar, v, to, S, boundLen)	



def intersect(matrix, grammar):
    graph_obj = lib.graph_new(matrix.encode('utf-8'))
    grammar_obj = lib.grammar_new(grammar.encode('utf-8'))
    lib.intersect(grammar_obj, graph_obj)
    return grammar_obj, graph_obj


def getelements(grammar_obj, S):
    elements = lib.get_elements(grammar_obj, S.encode('utf-8'))
    result = ctypes.cast(elements, ctypes.c_char_p).value
    lib.string_del(elements)
    return result.decode('utf-8')


def getpaths(grammar_obj, i, j, S, k):
    paths = lib.getpaths(grammar_obj, i, j, S.encode('utf-8'), k)
    result = ctypes.cast(paths, ctypes.c_char_p).value
    lib.string_del(paths)
    return result.decode('utf-8').split('\n')[:-1] #for last '\n'



apap = MyAllPairsAllPaths()
apap.create_index("matrix.txt","grammar.txt")
print(apap.restore_paths(0, 0, "s", 20))
print(apap.restore_paths(0, 0, "s", 40))
apap.create_index("matrix.txt","grammar.txt")
print(apap.restore_paths(0, 0, "s", 20))
del apap
exit(0)

lib.graphblas_init()
#start = time()
#mygrammar = intersect("g1.cnf", "../../CFPQ-Tensor-CPU/Py_algo/Matr_algo/pathways.txt", "result_pathways.txt")
#finish = time()
#print(str(finish - start))
#print(getelements(mygrammar, "s"))
#lib.grammar_del(mygrammar)
#lib.graphblas_finalize()
#exit(0)


name_graphs = "go.txt"
name_grammar = "g1.cnf"

mygrammar = intersect(name_grammar, "../../CFPQ-Tensor-CPU/Py_algo/Matr_algo/" + name_graphs, "result_matr/paths/result_" + name_graphs)
elements = getelements(mygrammar, "s")
for elem in getelements(mygrammar, "s").split("\n"):
    #print(elem)
    part_elem = elem.split(" ")
    print(part_elem)
    if len(part_elem) < 2:
        continue
    start = time()
    getpaths(mygrammar, int(part_elem[0]), int(part_elem[1]), "s", 20)
    finish = time()
    file = open("result_matr/paths/result_time_" + name_graphs, "a")
    file.write(str(finish - start) + "\n")


#print(str(finish - start))
#start = time()
#getpaths(mygrammar, 375, 375, "s", 20)
#finish = time()
#print(str(finish - start))
#lib.string_del(elements)
lib.grammar_del(mygrammar)
lib.graphblas_finalize()
exit(0)

from tqdm import tqdm

name_graphs = ["pathways"] #"go-hierarchy", "go", "geospecies", "taxonomy"] #"arch", "crypto", "drivers", "fs"] #"geospecies", "eclass_514en", "enzyme", "go-hierarchy", "go", "pathways", "taxonomy"]

for name_graph in tqdm(name_graphs):
    rounds = 1
    name_grammar = "g1.cnf" #name_graph + "_automat"

    #file_stat = open("result_matr/" + name_graph + "_stat.csv", "a")
    #file_stat.write("graph,grammar,min,max,mean,length_path,sum_S,rounds\n")
    result_time = []
    control_sum = 0
    length_path = 0
    mygrammar = 0
    for i in range(rounds):
        lib.graphblas_init()
        mygrammar = intersect(name_grammar, "../../CFPQ-Tensor-CPU/Py_algo/Matr_algo/" + name_graph + ".txt", "result_matr/result_" + name_graph + ".txt")
        #lib.grammar_del(mygrammar)
        #lib.graphblas_finalize()

    #start = time()
    #getpaths(mygrammar)
    #finish = time()
    #max_time = max(result_time)
    #min_time = min(result_time)
    #mean_time = sum(result_time) / len(result_time)

    #file_stat.write(name_graph + "," + "memory_aliases_automat" + "," + str(min_time) + "," + str(max_time) + ","
                    #+ str(mean_time) + "," + str(length_path) + "," + str(control_sum) + "," + str(rounds) + "\n")

    file_stat.close()

lib.graphblas_finalize()
