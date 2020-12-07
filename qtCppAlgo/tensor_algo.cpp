//
// Created by ilya on 05.12.2020.
//

#include "tensor_algo.h"

#include <malloc.h>

TensorAlgo::TensorAlgo(LabelGraph *new_graph, RecursiveAutomaton *new_rsa) {
    //cout << "OK";
    graph = new_graph;
    rsa = new_rsa;
    for(auto &lebal:rsa->getS())
    {
        graph->addMatrix(lebal);
    }

    sizeKron = graph->get_matrices_size() * rsa->getMatricesSize();

    OK(GrB_Matrix_new(&kron, GrB_BOOL, sizeKron, sizeKron));

//    for (auto &elem:rsa->getAutomaton())
//    {
//        cout << "\n" << elem.first << " ";
//        GrB_Index index;
//        OK(GrB_Matrix_ncols(&index, elem.second));
//        cout << index;
//    }



//    cout << "---------";

//    for (auto &elem:graph.get_matrices())
//    {
//        cout << "\n" << elem.first << " ";
//        GrB_Index index;
//        OK(GrB_Matrix_ncols(&index, elem.second));
//        cout << index;
//    }

//    graph.get_matrices();
}

TensorAlgo::~TensorAlgo()
{
    OK(GrB_Matrix_free(&kron));
}

GrB_Matrix *TensorAlgo::getKron()
{
    return &kron;
}

int TensorAlgo::solve() {


    for(auto &label: rsa->getStartAndFinish())
    {
        for(int i = 0; i < graph->get_matrices_size(); i++)
        {
            OK(GrB_Matrix_setElement_BOOL(graph->get_matrices()[label], true, i, i));
        }
    }

    GrB_Index *I = (GrB_Index *) malloc(graph->get_matrices_size() * sizeof (GrB_Index));
    GrB_Index *J = (GrB_Index *) malloc(graph->get_matrices_size() * sizeof (GrB_Index));

    GrB_Matrix calcKron;
    OK(GrB_Matrix_new(&calcKron, GrB_BOOL, sizeKron, sizeKron));

    int control_sum = 0;
    bool changed = true;
    while (changed)
    {
        changed = false;

        for(auto &label:rsa->getLabels())
        {
            OK(GrB_Matrix_clear(calcKron));
            OK(GxB_kron(calcKron, NULL, NULL, GrB_LAND, rsa->getAutomaton()[label], graph->get_matrices()[label], NULL));
            OK(GrB_eWiseAdd_Matrix_BinaryOp(kron, NULL, NULL, GrB_LOR, kron, calcKron, NULL));
        }

        GrB_Index prev;
        OK(GrB_Matrix_nvals(&prev, kron));
        GrB_Matrix degree;
        OK(GrB_Matrix_new(&degree, GrB_BOOL, sizeKron, sizeKron));
        OK(GrB_Matrix_dup(&degree, kron));
        bool transitive_changed = true;
        while (transitive_changed)
        {
            transitive_changed = false;
            OK(GrB_mxm(degree, NULL, NULL, GxB_LOR_LAND_BOOL, degree, kron, NULL));
            OK(GrB_eWiseAdd_Matrix_BinaryOp(kron, NULL, NULL, GrB_LOR, kron, degree, NULL));

            GrB_Index cur;
            OK(GrB_Matrix_nvals(&cur, kron));
            if (prev != cur)
            {
                prev = cur;
                transitive_changed = true;
            }
        }

        OK(GrB_Matrix_free(&degree));

        for (auto &elem:rsa->getStates())
        {
            string label = elem.first;
            for (auto &element:elem.second)
            {
                int start_i = element.first * graph->get_matrices_size();
                int start_j = element.second * graph->get_matrices_size();

                for (int index = 0; index < graph->get_matrices_size(); index++)
                {
                    I[index] = start_i + index;
                    J[index] = start_j + index;
                }

                GrB_Matrix block;
                OK(GrB_Matrix_new(&block, GrB_BOOL, graph->get_matrices_size(), graph->get_matrices_size()));
                OK(GrB_Matrix_extract(block, NULL, NULL, kron, I, graph->get_matrices_size(), J, graph->get_matrices_size(), NULL));
                OK(GrB_eWiseAdd_Matrix_BinaryOp(graph->get_matrices()[label], NULL, NULL, GrB_LOR, graph->get_matrices()[label], block, NULL));

                GrB_Index new_control_sum;
                OK(GrB_Matrix_nvals(&new_control_sum, graph->get_matrices()[label]));
                if (new_control_sum != control_sum)
                {
                    changed = true;
                    control_sum = new_control_sum;
                }

                OK(GrB_Matrix_free(&block));
            }
        }

    }

    OK(GrB_Matrix_free(&calcKron));
    free(I);
    free(J);

    return control_sum;
}
