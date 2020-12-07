#include <iostream>
#include <string>

extern "C"
{
#include "GraphBLAS.h"
}

#include "labelgraph.h"
#include "rsa.h"
#include "tensor_algo.h"
#include "tensor_paths.h"

using namespace std;

int main()
{
    GrB_init(GrB_NONBLOCKING);

    LabelGraph graph("graph");

//    cout << graph.get_matrices_size() << "\n";

//    for (auto &label:graph.getLabels())
//        cout << label << " ";

    RecursiveAutomaton rsa("rsa");

//    for (auto &elem:rsa.getS())
//        cout << elem;


//    cout << "----------";

//    for (auto &elem:rsa.getAutomaton())
//    {
//        cout << "\n" << elem.first << " ";
//        GrB_Index index;
//        OK(GrB_Matrix_ncols(&index, elem.second));
//        cout << index;
//    }
//    cout << "_______________asdasdasd\n";
//    for (auto &elem:rsa.getStates())
//    {
//        cout << elem.first;
//        for (auto &elem_vec:elem.second)
//        {
//            cout << "\n" << elem_vec.first << " " << elem_vec.second;
//        }
//    }
//    cout << "\n" << rsa.getMatricesSize() << "\n";

//    for(auto &elem:rsa.getStates())
//    {
//        cout << elem.first << "\n";
//        for (auto &elem_vec:elem.second)
//        {
//            cout << elem_vec.first << " " << elem_vec.second << "\n";
//        }
//    }

//    bool elem = false;
//    GrB_Matrix_extractElement_BOOL(&elem, rsa.getAutomaton()["a"], 0, 1);
//    cout << "\n" << elem << "\n";

    TensorAlgo algo(&graph, &rsa);

    auto res = algo.solve();

    cout << res;

//    for (auto &matrix:graph.get_matrices())
//    {
//        cout << matrix.first << "\n";
//        for (int i = 0; i < graph.get_matrices_size(); i++)
//        {
//            for (int j = 0; j < graph.get_matrices_size(); j++)
//            {
//                bool elem = false;
//                GrB_Matrix_extractElement_BOOL(&elem, matrix.second, i, j);
//                cout << elem << " ";
//            }
//            cout << "\n";
//        }
//    }
    cout << "\n\n";

    TensorPaths paths(&graph, &rsa, algo.getKron());

    auto res_paths = paths.get_paths(2, 2, "S", 25);

    for (auto &path:res_paths)
    {
        cout << path.first << "\n";
    }

    OK(GrB_finalize());
    return 0;
}
