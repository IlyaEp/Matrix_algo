#ifndef CFPQ_CFPQ_H
#define CFPQ_CFPQ_H

#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <iostream>
#include <vector>
#include "apmatrix.h"
#include "Graph.h"

using nonterminals_pair = std::pair<unsigned int, unsigned int>;

class Grammar {

public:
    explicit Grammar(const std::string &grammar_filename);

    ~Grammar();

    char* get_elements(std::string S);

    unsigned int intersection_with_graph(Graph &graph) {
        vertices_count = graph.vertices_count;
        matrices.reserve(nonterminals_count);
        rules_with_nonterminal.reserve(nonterminals_count);

        for (unsigned int i = 0; i < nonterminals_count; ++i) {
            rules_with_nonterminal.emplace_back();
            matrices.push_back(new ApMatrix(vertices_count));
        }

        for (unsigned int i = 0; i < rules.size(); ++i) {
            rules_with_nonterminal[rules[i].second.first].push_back(i);
            rules_with_nonterminal[rules[i].second.second].push_back(i);
            to_recalculate.insert(i);
        }

        for (auto &edge : graph.edges) {
            for (unsigned int nonterm : terminal_to_nonterminals[edge.first]) {
                matrices[nonterm]->set_bit(edge.second.first, edge.second.second);
            }
        }

        using namespace std::chrono;
        high_resolution_clock::time_point begin_time = high_resolution_clock::now();

        while (!to_recalculate.empty()) {
            unsigned int rule_index = *to_recalculate.begin();
            to_recalculate.erase(to_recalculate.begin());
            unsigned int C = rules[rule_index].first;
            unsigned int A = rules[rule_index].second.first;
            unsigned int B = rules[rule_index].second.second;
            //std::cout << C << " " << A << " " << B << std::endl;
            if (matrices[C]->add_mul(matrices[A], matrices[B])) {
                for (unsigned int changed_rule_index: rules_with_nonterminal[C]) {
                    to_recalculate.insert(changed_rule_index);
                }
            }
        }
        high_resolution_clock::time_point end_time = high_resolution_clock::now();
        milliseconds elapsed_secs = duration_cast<milliseconds>(end_time - begin_time);
        //std::cout << elapsed_secs.count() << " ";
        return static_cast<unsigned int>(elapsed_secs.count());
    }

    /*def matrix_get_single_path(m: IndexGraph, grammar: CnfGrammar, i, j, s):
        def is_identity(lft, r, mid, h, lng):
            return lft == 0 and r == 0 and mid == 0 and h == 0 and lng == 0

        left, right, middle, height, length = m[s].get(i, j)

        if is_identity(left, right, middle, height, length):
            print("Index isn`t correct\n")
            return

        if height == 1:
            print("({0}-[:{1}]->{2})".format(left, s, right))
            return

        for l, r1, r2 in grammar.complex_rules:
            if s == l:
                left_r1, right_r1, middle_r1, height_r1, length_r1 = m[r1].get(left, middle)
                left_r2, right_r2, middle_r2, height_r2, length_r2 = m[r2].get(middle, right)

                if not is_identity(left_r1, right_r1, middle_r1, height_r1, length_r1) and \
                        not is_identity(left_r2, right_r2, middle_r2, height_r2, length_r2):
                    max_height = height_r2 if height_r1 < height_r2 else height_r1
                    if height == max_height + 1:
                        matrix_get_single_path(m, grammar, left, middle, r1)
                        matrix_get_single_path(m, grammar, middle, right, r2)
                        break

    */

    /*GrB_Index *PathIndex_MatrixGetPath(const GrB_Matrix *matrices, const Grammar *grammar, GrB_Index left, GrB_Index right, MapperIndex nonterm) {
        PathIndex index;
        PathIndex_InitIdentity(&index);
        GrB_Matrix_extractElement((void *) &index, matrices[nonterm], left, right);

        GrB_Index *path = array_new(GrB_Index, index.length + 1);

        PathIndex *sp = array_new(PathIndex, index.length + 1);
        MapperIndex *nonterms = array_new(MapperIndex, index.length + 1);
        uint8_t *directions = array_new(uint8_t, index.length + 1);

        sp = array_append(sp, index);
        nonterms = array_append(nonterms, nonterm);
        directions = array_append(directions, 0);

        while (array_len(sp) != 0) {
            PathIndex top = sp[array_len(sp)-1];
            nonterm = nonterms[array_len(nonterms)-1];
            uint8_t direction = directions[array_len(directions)-1];

            if (PathIndex_IsEdge(&top) || direction == 2) {
                if (PathIndex_IsEdge(&top))
                    path = array_append(path, top.left);

                array_pop(sp);
                array_pop(nonterms);
                array_pop(directions);

                if (array_len(directions) != 0)
                    directions[array_len(directions)-1] += 1;
                else
                    break;
            } else {
                for (GrB_Index i = 0; i < grammar->complex_rules_count; ++i) {
                    MapperIndex nonterm_l = grammar->complex_rules[i].l;
                    MapperIndex nonterm_r1 = grammar->complex_rules[i].r1;
                    MapperIndex nonterm_r2 = grammar->complex_rules[i].r2;

                    if (nonterm == nonterm_l) {
                        PathIndex index_r1, index_r2;
                        PathIndex_InitIdentity(&index_r1);
                        PathIndex_InitIdentity(&index_r2);
                        GrB_Matrix_extractElement((void *) &index_r1, matrices[nonterm_r1], top.left, top.middle);
                        GrB_Matrix_extractElement((void *) &index_r2, matrices[nonterm_r2], top.middle, top.right);

                        if (!PathIndex_IsIdentity(&index_r1) && !PathIndex_IsIdentity(&index_r2)) {
                            uint32_t max_height = index_r1.height < index_r2.height ? index_r2.height : index_r1.height;
                            if (top.height == max_height + 1) {
                                if (direction == 0) {
                                    sp = array_append(sp, index_r1);
                                    nonterms = array_append(nonterms, nonterm_r1);
                                } else {
                                    sp = array_append(sp, index_r2);
                                    nonterms = array_append(nonterms, nonterm_r2);
                                }
                                directions = array_append(directions, 0);
                                break;
                            }
                        }
                    }
                }
            }
        }
        array_free(sp);
        array_free(nonterms);
        array_free(directions);
        return path;
    }*/

    std::vector<std::pair<std::string, unsigned int>> get_paths(unsigned int i, unsigned int j, const std::string &nonterm_name, unsigned int current_len) {
        if (current_len > 0)
        {
            unsigned int S = nonterminal_to_index[nonterm_name];
            ApMatrix* m = matrices[S];
            unsigned int size = m->get_size();
            PathIndex* pindex = m->get_bit(i, j);
            if (PathIndex_IsIdentity(pindex))
            {
                return std::vector<std::pair<std::string, unsigned int>>();
            }
            unsigned int k = 0;
            std::vector<std::pair<std::string, unsigned int>> res = std::vector<std::pair<std::string, unsigned int>>();
            for (unsigned int mid = 0; mid < pindex->size; mid++)
            {
                k = pindex->middle[mid];
                if (k == size)
                {
                    res.push_back(std::make_pair("(" + std::to_string(i) + "->" + std::to_string(j) + ")", 1));
                }
                else
                {
                    for (auto rule : rules)
                    {
                        if(rule.first == S)
                        {
                            unsigned int A = rule.second.first;
                            unsigned int B = rule.second.second;
                            std::string A_name, B_name;
                            for (auto &nonterm : nonterminal_to_index)
                            {
                                if (nonterm.second == A)
                                {
                                    A_name = nonterm.first;
                                }
                                else if (nonterm.second == B)
                                {
                                    B_name = nonterm.first;
                                }
                            }
                            std::vector<std::pair<std::string, unsigned int>> left_paths = get_paths(i, k, A_name, current_len - 1);
                            if(left_paths.size() > 0)
                            {
                                unsigned int min_len = current_len;
                                for (auto p : left_paths)
                                {
                                    if (p.second < min_len)
                                    {
                                        min_len = p.second;
                                    }
                                }
                                std::vector<std::pair<std::string, unsigned int>> right_paths = get_paths(k, j, B_name, current_len - min_len);
                                for(auto p1 : left_paths)
                                {
                                    for(auto p2 : right_paths)
                                    {
                                        if (p1.second + p2.second < current_len)
                                        {
                                            res.push_back(std::make_pair(p1.first + p2.first, p1.second + p2.second));
                                        }
                                    }
                                }
                            }

                        }
                    }
                }

            }

            delete pindex;

            return res;
        }

        return std::vector<std::pair<std::string, unsigned int>>();
    }

    void print_results(const std::string &output_filename, int time);

    unsigned int nonterminals_count = 0;
    unsigned int vertices_count = 0;

    std::unordered_set<unsigned int> to_recalculate;
    std::vector<std::vector<unsigned int>> rules_with_nonterminal;
    std::map<std::string, unsigned int> nonterminal_to_index;
    std::unordered_map<std::string, std::vector<int>> terminal_to_nonterminals;
    std::vector<std::pair<unsigned int, nonterminals_pair>> rules;
    std::vector<ApMatrix *> matrices;
};

#endif //CFPQ_CFPQ_H

