//
// Created by ilya on 06.12.2020.
//

#include "tensor_paths.h"

void print(vector<pair<string, int>> v)
{
    for (auto &elem:v)
    {
        cout << "(" << elem.first << ") size " << elem.second << "\n";
    }
}

TensorPaths::TensorPaths(LabelGraph *new_graph, RecursiveAutomaton *new_rsa, GrB_Matrix *new_tc) {
    graph = new_graph;
    rsa = new_rsa;
    GrB_Index size_tc;
    OK(GrB_Matrix_ncols(&size_tc, *new_tc));

    OK(GrB_transpose(*new_tc, NULL, NULL, *new_tc, NULL));

    GrB_Index *I = (GrB_Index *)malloc(sizeof (GrB_Index) * size_tc);
    for (int i = 0; i < size_tc; i++)
    {
        I[i] = i;
    }

    tc = map<int, vector<int>>();
    for(int i = 0; i < size_tc; i++)
    {
        GrB_Vector new_row;
        OK(GrB_Vector_new(&new_row, GrB_BOOL, size_tc));
        OK(GrB_Col_extract(new_row, NULL, NULL, *new_tc, I, size_tc, i, NULL));
        vector<int> new_element_tc;
        for (int j = 0; j < size_tc; j++)
        {
            bool elem = false;
            GrB_Vector_extractElement_BOOL(&elem, new_row, j);
            if (elem)
                new_element_tc.push_back(j);
        }

        tc[i] = new_element_tc;
        OK(GrB_Vector_free(&new_row));
    }

    for (int i = 0; i < rsa->getMatricesSize(); i++)
    {
        for(int j = 0; j < rsa->getMatricesSize(); j++)
        {
            bool check = true;
            for (auto &elem:rsa->getAutomaton())
            {
                bool element = false;
                GrB_Matrix_extractElement_BOOL(&element, elem.second, i, j);
                if (element)
                {
                    check = false;
                    break;
                }
            }

            if (check)
                skip_rsa.insert(make_pair(i, j));
        }
    }

//    for (auto &elem:skip_rsa)
//        cout << elem.first << " " << elem.second << "\n";

    free(I);
}

vector<pair<string, int>> TensorPaths::get_paths(int start, int finish, string nonterm, int current_len) {
    int start_state = rsa->getStartState()[nonterm];
    vector<int> finish_states = rsa->getFinishStates()[nonterm];

    vector<pair<string, int>> result;
    for (auto &f: finish_states)
    {
        auto new_result = bfs(start_state * graph->get_matrices_size() + start, f * graph->get_matrices_size() + finish, current_len);
//        cout << "get_paths\n";
//        print(result);
//        cout << "\n\n";
        move(new_result.begin(), new_result.end(), back_inserter(result));
    }

    return result;
}

vector<pair<string, int>> TensorPaths::bfs(int i, int j, int current_len) {
    if(current_len < 1)
        return vector<pair<string, int>>();

    vector<pair<string, int>> result_paths;
    for(auto &elem:tc[i])
    {
        GrB_Index graph_i = i % graph->get_matrices_size();
        GrB_Index graph_j = elem % graph->get_matrices_size();

        GrB_Index rsa_i = i / graph->get_matrices_size();
        GrB_Index rsa_j = elem / graph->get_matrices_size();

        auto check_skip = skip_rsa.find(make_pair(rsa_i, rsa_j));
        if (check_skip != skip_rsa.end())
            continue;

        vector<pair<string, int>> left_paths;
        for (auto &label:rsa->getTerminals())
        {
            bool check_graph = false;
            GrB_Matrix_extractElement_BOOL(&check_graph, graph->get_matrices()[label], graph_i, graph_j);
            if (check_graph)
            {
                bool check_rsa = false;
                GrB_Matrix_extractElement_BOOL(&check_rsa, rsa->getAutomaton()[label], rsa_i, rsa_j);
                if (check_rsa)
                    left_paths.emplace_back(to_string(graph_i) + " " + to_string(graph_j), 1);
            }
        }

//        cout << "bfs 1\n";
//        print(left_paths);
//        cout << "\n\n";

        for (auto &nonterm:rsa->getS())
        {
            bool check_graph = false;
            GrB_Matrix_extractElement_BOOL(&check_graph, graph->get_matrices()[nonterm], graph_i, graph_j);
            if (check_graph)
            {
                bool check_rsa = false;
                GrB_Matrix_extractElement_BOOL(&check_rsa, rsa->getAutomaton()[nonterm], rsa_i, rsa_j);
                if (check_rsa)
                {
                    auto new_result = get_paths(graph_i, graph_j, nonterm, current_len - 1);
                    move(new_result.begin(), new_result.end(), back_inserter(left_paths));
//                    cout << "bfs 2\n";
//                    print(left_paths);
//                    cout << "\n\n";
                }
            }
        }

        if (left_paths.size() == 0)
            continue;

        int min_len = current_len;
        for (auto &path:left_paths)
        {
            if (path.second < min_len)
                min_len = path.second;
        }

        vector<pair<string, int>> right_paths;
        if (elem != j)
            right_paths = bfs(elem, j, current_len - min_len);
        else
        {
            right_paths.emplace_back("", 0);
        }

//        cout << "bfs 3\n";
//        print(right_paths);
//        cout << "\n\n";

        for (auto &left: left_paths)
        {
            for (auto &right: right_paths)
            {
//                cout << "product\n";
//                cout << left.first << "\n";
//                cout << right.first << "\n";
                if (left.second + right.second < current_len)
                    result_paths.emplace_back(left.first + " " + right.first, left.second + right.second);
            }
        }
    }

    //cout << "bfs 4\n";
    //print(result_paths);
    //cout << "\n\n";

    return result_paths;
}
