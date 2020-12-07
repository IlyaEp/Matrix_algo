//
// Created by ilya on 05.12.2020.
//

#include <fstream>
#include <algorithm>

#include "rsa.h"

set<string> RecursiveAutomaton::getLabels() {
    return labels;
}

set<string> RecursiveAutomaton::getS() {
    return S;
}

map<string, GrB_Matrix> RecursiveAutomaton::getAutomaton() {
    return automaton;
}

set<string> RecursiveAutomaton::getStartAndFinish()
{
    return start_and_finish;
}

set<string> RecursiveAutomaton::getTerminals()
{
    return terminals;
}

RecursiveAutomaton::RecursiveAutomaton(const string& path) {
    auto rsa_stream = ifstream(path);

    int count_matrix = 0;
    rsa_stream >> count_matrix;
    count_automaton = count_matrix;

    int count_s = 0;
    rsa_stream >> count_s;

    int size_matrix = 0;
    rsa_stream >> size_matrix;
    matrices_size = size_matrix;

    for(int i = 0; i < count_matrix; i++)
    {
        string label;
        rsa_stream >> label;
        labels.insert(label);
        GrB_Matrix new_matrix;
        OK(GrB_Matrix_new(&new_matrix, GrB_BOOL, size_matrix, size_matrix));

        int count_edge = 0;
        rsa_stream >> count_edge;
        for(int j = 0; j < count_edge; j++)
        {
            int from = 0;
            int to = 0;
            rsa_stream >> from >> to;
            GrB_Index index_to = to;
            GrB_Index index_from = from;
            OK(GrB_Matrix_setElement_BOOL(new_matrix, true, index_from, index_to));
        }

        automaton[label] = new_matrix;
    }

    for(int i = 0; i < count_s; i++)
    {
        string label;
        rsa_stream >> label;
        S.insert(label);
        vector<pair<int, int>> new_state;

        int count_edges = 0;
        rsa_stream >> count_edges;
        for(int j = 0; j < count_edges; j++)
        {
            int from = 0;
            int to = 0;
            rsa_stream >> from >> to;
            new_state.emplace_back(from, to);

            start_state[label] = from;
            if(finish_states.find(label) == finish_states.end())
            {
                vector<int> new_vector = {to};
                finish_states[label] = new_vector;
            } else {
                finish_states[label].push_back(from);
            }

            if(from == to)
                start_and_finish.insert(label);
        }

        states[label] = new_state;
    }

    set_difference(labels.begin(), labels.end(),
                   S.begin(), S.end(),
                   inserter(terminals, terminals.begin()));
}

RecursiveAutomaton::RecursiveAutomaton() {
    count_automaton = 0;
    matrices_size = 0;
    labels = set<string>();
    S = set<string>();
    terminals = set<string>();
    finish_states = map<string, vector<int>>();
    start_state = map<string, int>();
    automaton = map<string, GrB_Matrix>();
    states = map<string, vector<pair<int, int>>>();
}

map<string, int> RecursiveAutomaton::getStartState() {
    return start_state;
}

map<string, vector<int>> RecursiveAutomaton::getFinishStates() {
    return finish_states;
}

int RecursiveAutomaton::getMatricesSize() {
    return matrices_size;
}

map<string, vector<pair<int, int>>> RecursiveAutomaton::getStates()
{
    return states;
}

RecursiveAutomaton::~RecursiveAutomaton()
{
    for (auto &elem: automaton)
        OK(GrB_Matrix_free(&elem.second));
}
