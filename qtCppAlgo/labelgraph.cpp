//
// Created by ilya on 04.12.2020.
//

#include <fstream>
#include <tuple>
#include <vector>

#include "labelgraph.h"

LabelGraph::LabelGraph(string path) {
    matrices = map<string, GrB_Matrix>();
    auto graph_stream = ifstream(path);
    vector<tuple<int, string, int>> edges;
    int from = 0;
    int to = 0;
    string label;
    int graph_size = 0;

    while(graph_stream >> from >> label >> to)
    {
        edges.emplace_back(from, label, to);
        graph_size = max(graph_size, max(from, to) + 1);
        labels.insert(label);
    }
    matrices_size = graph_size;

    for (const auto& l:labels)
    {
        GrB_Matrix new_matrix;
        OK(GrB_Matrix_new(&new_matrix, GrB_BOOL, matrices_size, matrices_size));
        matrices[l] = new_matrix;
    }

    for(auto &edge : edges)
    {
        const auto& [from, label, to] = edge;
        GrB_Index index_from = from;
        GrB_Index index_to = to;
        OK(GrB_Matrix_setElement_BOOL(matrices[label], true, index_from, index_to));
    }
}

map<string, GrB_Matrix> LabelGraph::get_matrices() {
//    for(auto &label:matrices)
//    {
//        cout << "\n" << label.first;
//    }
    return matrices;
}

LabelGraph::~LabelGraph() {
    for (const auto& label:labels)
    {
        OK(GrB_Matrix_free(&matrices[label]));
    }
}

int LabelGraph::get_matrices_size() {
    return matrices_size;
}

LabelGraph::LabelGraph() {
matrices_size = 0;
labels = set<string>();
matrices = map<string, GrB_Matrix>();
}

set<string> LabelGraph::getLabels()
{
    return labels;
}

void LabelGraph::addMatrix(string label)
{
    GrB_Matrix new_matrix;
    OK(GrB_Matrix_new(&new_matrix, GrB_BOOL, matrices_size, matrices_size));
    matrices[label] = new_matrix;
    labels.insert(label);
}
