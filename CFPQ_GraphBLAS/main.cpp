#include <iostream>
#include <chrono>
#include <fstream>
#include <fstream>
extern "C" {
#include "GraphBLAS.h"
}
#include <stdio.h>
#include "Grammar.h"
#include "Graph.h"
#include "apmatrix.h"
#include "pathindex.h"
using namespace std;

extern "C" {

void string_del(char *elements)
{
    free(elements);
}

Grammar *grammar_new(char *name) {
    return new Grammar(name);
}

void grammar_del(Grammar* grammar)
{
    delete grammar;
}

Graph *graph_new(char *name) {
    return new Graph(name);
}

void graph_del(Graph* graph)
{
    delete graph;
}

void graphblas_init()
{
    GrB_init(GrB_NONBLOCKING);
    InitGBSemiring();
}

void graphblas_finalize()
{
    GrB_Semiring_free(&IndexType_Semiring);
    GrB_Monoid_free(&IndexType_Monoid);
    GrB_BinaryOp_free(&IndexType_Add);
    GrB_BinaryOp_free(&IndexType_Mul);
    GrB_Type_free(&PathIndexType);
    GrB_finalize();
}

void intersect(Grammar* grammar, Graph* graph)
{
    //Graph graph = Graph(matrix);
    //auto begin = std::chrono::steady_clock::now();
    auto times = grammar->intersection_with_graph(*graph);
    //auto end = std::chrono::steady_clock::now();
    //auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    //grammar->print_results(argv[1], elapsed_ms.count());
}

char* get_elements(Grammar* grammar, char *S)
{
    return grammar->get_elements(S);
}

char* getpaths(Grammar* grammar, int i, int j, char *S, int current_len)
{
    std::vector<std::pair<std::string, unsigned int>> res = grammar->get_paths(i, j, S, current_len);
    char* result = (char *)malloc(res.size()*25*current_len*sizeof(char));    //???
    std::string start = "";
    strcpy(result, start.c_str());
    for (auto path : res)
    {

        std::string path_str = path.first + "\n";
        strcat(result, path_str.c_str());
    }
    //string name_graphs = "go.txt";
    //std::ofstream file;
    //file.open("result_matr/paths/result_paht_" + name_graphs, ios_base::app);
    //file << res.size() << "\n";
    //file.close();
    
    return result;   
}

}

int main(int argc, char *argv[]) {
    //getpaths(argc, argv, 0, 0, 0, 10);
    return 0;
}

