//
// Created by ilya on 05.12.2020.
//

#ifndef CFPQ_CPPALGO_TENSOR_ALGO_H
#define CFPQ_CPPALGO_TENSOR_ALGO_H

#include <tuple>

#include "rsa.h"
#include "labelgraph.h"

extern "C"
{
#include <GraphBLAS.h>
};

#define OK(method) do { GrB_Info info = method;					\
    if (info != GrB_SUCCESS) {						\
      printf("%s:%d:GB: %d: %s\n", __FILE__, __LINE__, info, GrB_error()); \
      exit(info); } } while (0)

class TensorAlgo {
private:
    LabelGraph *graph;
    RecursiveAutomaton *rsa;
    GrB_Matrix kron;
    int64_t sizeKron;
public:
    TensorAlgo(LabelGraph *new_graph, RecursiveAutomaton *new_rsa);
    int solve();
    ~TensorAlgo();
    GrB_Matrix *getKron();
};


#endif //CFPQ_CPPALGO_TENSOR_ALGO_H
