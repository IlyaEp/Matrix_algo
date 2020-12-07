//
// Created by ilya on 05.12.2020.
//

#ifndef CFPQ_CPPALGO_RSA_H
#define CFPQ_CPPALGO_RSA_H

#include <map>
#include <set>
#include <vector>
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

class RecursiveAutomaton {
private:
    set<string> labels;
    set<string> S;
    set<string> terminals;
    map<string, vector<int>> finish_states;
    map<string, int> start_state;
    map<string, GrB_Matrix> automaton;
    map<string, vector<pair<int, int>>> states;
    int matrices_size;
    int count_automaton;
    set<string> start_and_finish;
public:
    set<string> getS();

    set<string> getLabels();

    map<string, GrB_Matrix> getAutomaton();

    map<string, int> getStartState();

    map<string, vector<int>> getFinishStates();

    int getMatricesSize();

    set<string> getStartAndFinish();

    map<string, vector<pair<int, int>>> getStates();

    set<string> getTerminals();

    RecursiveAutomaton(const string& path);

    RecursiveAutomaton();
    ~RecursiveAutomaton();

};


#endif //CFPQ_CPPALGO_RSA_H
