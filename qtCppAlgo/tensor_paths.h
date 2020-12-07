//
// Created by ilya on 06.12.2020.
//

#ifndef CFPQ_CPPALGO_TENSOR_PATHS_H
#define CFPQ_CPPALGO_TENSOR_PATHS_H

extern "C"
{
#include <GraphBLAS.h>
};

#define OK(method) do { GrB_Info info = method;					\
    if (info != GrB_SUCCESS) {						\
      printf("%s:%d:GB: %d: %s\n", __FILE__, __LINE__, info, GrB_error()); \
      exit(info); } } while (0)

#include <vector>
#include <string>
#include <utility>

#include "labelgraph.h"
#include "rsa.h"

class TensorPaths {
private:
    LabelGraph *graph;
    RecursiveAutomaton *rsa;
    map<int, vector<int>> tc;
    set<pair<int, int>> skip_rsa;
public:
    TensorPaths(LabelGraph *new_graph, RecursiveAutomaton *new_rsa, GrB_Matrix *new_tc);
    vector<pair<string, int>> get_paths(int start, int finish, string nonterm, int current_len);
    vector<pair<string, int>> bfs(int i, int j, int current_len);
};


#endif //CFPQ_CPPALGO_TENSOR_PATHS_H
