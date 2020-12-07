//
// Created by ilya on 04.12.2020.
//

#ifndef CFPQ_CPPALGO_LABELGRAPH_H
#define CFPQ_CPPALGO_LABELGRAPH_H

#include <iostream>
#include <map>
#include <set>
#include <string>

extern "C"
{
#include <GraphBLAS.h>
};

using namespace std;

#define OK(method) do { GrB_Info info = method;					\
    if (info != GrB_SUCCESS) {						\
      printf("%s:%d:GB: %d: %s\n", __FILE__, __LINE__, info, GrB_error()); \
      exit(info); } } while (0)

class LabelGraph{
private:
    static const int MAX_MATRIX_SIZE = 1000000;
    map<string, GrB_Matrix> matrices;
    set<string> labels;
    int matrices_size;
public:
    LabelGraph();
LabelGraph(string path);
~LabelGraph();
map<string, GrB_Matrix> get_matrices();
int get_matrices_size();
set<string> getLabels();
void addMatrix(string label);
};


#endif //CFPQ_CPPALGO_LABELGRAPH_H
