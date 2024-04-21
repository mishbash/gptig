#include "ig_lupanov_synthesis.h"

#include <vector>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <cassert>
#include <cmath>


IGLupanovSynthesis::IGLupanovSynthesis(Function function) : function(function) {
    variables_count = function.getVariableCount();

    unsigned log2n = 1;
    while ((1 << (log2n + 1)) < variables_count) {
        ++log2n;
    }

    k_first = log2n;
    nk_last = variables_count - log2n;

    assert(k_first > 0);
    assert(nk_last > 0);

    block_width = variables_count - 2 * log2n;
    assert(block_width > 0);
    block_lines = ((1 << k_first) + block_width - 1) / block_width;
}
 
void IGLupanovSynthesis::build_conjunction_set_naive() {
    Function Conjunction({0,0,0,1});

    auto start_period = scheme.getLastNodeNum();
                        scheme.addNode(IGNode({1, 1}, {0, 0}, Conjunction));
    auto stop_period =  scheme.addNode(IGNode({1, 1}, {1, 1}, Conjunction));

    for (auto cur_input : std::views::iota(2u, k_first + 1)) {
        for (auto binary_set : std::views::iota(start_period + 1, stop_period + 1))
            scheme.addNode(IGNode({binary_set, cur_input}, {0, 0}, Conjunction));

        for (auto binary_set : std::views::iota(start_period + 1, stop_period + 1))
            scheme.addNode(IGNode({binary_set, cur_input}, {0, 1}, Conjunction));

        start_period = stop_period;
        stop_period = scheme.getLastNodeNum();
    }


    border_before_cus = start_period + 1;
    border_between_cus = stop_period;

    start_period = scheme.getLastNodeNum();

                    scheme.addNode(IGNode({k_first + 1, k_first + 1}, {0, 0}, Conjunction));
    stop_period =   scheme.addNode(IGNode({k_first + 1, k_first + 1}, {1, 1}, Conjunction));

    for (auto cur_input : std::views::iota(k_first + 2, variables_count + 1)) {
        for (auto binary_set : std::views::iota(start_period + 1, stop_period + 1)) 
            scheme.addNode(IGNode({binary_set, cur_input}, {0, 0}, Conjunction));
        
        for (auto binary_set : std::views::iota(start_period + 1, stop_period + 1)) 
            scheme.addNode(IGNode({binary_set, cur_input}, {0, 1}, Conjunction));

        start_period = stop_period;
        stop_period = scheme.getLastNodeNum();
    }

    border_start_of_second_cus = start_period + 1;
    border_of_cus = stop_period;
}

int IGLupanovSynthesis::build_pdnf(std::vector<IGLupanovSubFunction>& disjuncts) {
    std::vector<int> nodes_to_add;

    for (auto disjunct : disjuncts) {
        if (!disjunct.value) continue;

        bool k_bitmask = 0;

        for (auto i : std::views::iota(0u, k_first)) {
            k_bitmask |= disjunct.truth_table[i] ? (1 << i) : 0; // !!!!
        }

        nodes_to_add.push_back(border_before_cus + k_bitmask);
    }

    if (nodes_to_add.size() == 0) {
        return -1;
    }

    if (nodes_to_add.size() == 1) {
        return nodes_to_add[0];
    }    

    for (int i = 1; i < nodes_to_add.size(); ++i) {
        if (i == 1) {
            auto num = scheme.addNode(IGNode({nodes_to_add[0], nodes_to_add[1]}, {1, 1}, {0, 0, 0, 1}));
            scheme.addNode(IGNode({num, num}, {1, 1}, {0, 0, 0, 1}));
        } else {
            auto num = scheme.addNode(IGNode({scheme.getLastNodeNum(), nodes_to_add[i]}, {1, 1}, {0, 0, 0, 1}));
            scheme.addNode(IGNode({num, num}, {1, 1}, {0, 0, 0, 1}));
        }
    }

    return scheme.getLastNodeNum();
}

void IGLupanovSynthesis::build() {
    scheme.setVariableCount(variables_count);
    build_conjunction_set_naive();


    // setup column per one secondary bitset

    std::vector<int> disjuncts_nodes;
    for (auto nk_bitmask : std::views::iota(0u, 1u << nk_last)) { // (... V ... V ... V ...) & bitset
        std::vector<IGLupanovSubFunction> disjuncts;
        std::vector<bool> binary_set(variables_count, 0);
        for (auto i : std::views::iota(0u, nk_last)) {
            binary_set[i + k_first] = nk_bitmask & (1 << i);
        }

        for (auto k_bitmask: std::views::iota(0u, 1u << k_first)) {
            std::vector<bool> disjunct_truth_table(k_first);
            for (auto i : std::views::iota(0u, k_first)) {
                binary_set[i]           = k_bitmask & (1 << i);
                disjunct_truth_table[i] = k_bitmask & (1 << i);
            }   
            
            auto res = function.getValue(binary_set);
            disjuncts.push_back({disjunct_truth_table, res});
        }

        /* SOME OUTPUT FOR TEST*/
        // std::cout << "For secondary mask: ";
        // for (auto i : std::views::iota(0u, nk_last)) {
        //     std::cout << binary_set[i + k_first];
        // }
        // std::cout << "\n";


        // for every colomn we calculate disjunct
        // then combine with nk_last
        
        auto node_to_add = build_pdnf(disjuncts);


        if (node_to_add != -1) {
            auto last = scheme.addNode(IGNode({node_to_add, border_start_of_second_cus + nk_bitmask}, {0, 0}, {0,0,0,1}));
            disjuncts_nodes.push_back(last);
        }   
    }


    if (disjuncts_nodes.size() == 0) {
        scheme.addNode(IGNode({1, 1}, {1, 0}, {0, 0, 0, 1})); // all 0 function
        scheme.setOutput(scheme.getLastNodeNum());
        return;
    }

    if (disjuncts_nodes.size() == 1) {
        scheme.setOutput(scheme.getLastNodeNum());
        return ;
    }    

    for (int i = 1; i < disjuncts_nodes.size(); ++i) {
        if (i == 1) {
            auto num = scheme.addNode(IGNode({disjuncts_nodes[0], disjuncts_nodes[1]}, {1, 1}, {0, 0, 0, 1}));
            scheme.addNode(IGNode({num, num}, {1, 1}, {0, 0, 0, 1}));
        } else {
            auto num = scheme.addNode(IGNode({scheme.getLastNodeNum(), disjuncts_nodes[i]}, {1, 1}, {0, 0, 0, 1}));
            scheme.addNode(IGNode({num, num}, {1, 1}, {0, 0, 0, 1}));
        }
    }

    scheme.setOutput(scheme.getLastNodeNum());

    // info();

}

void IGLupanovSynthesis::info() {
    std::cout << "var_count: " << variables_count << "\n";
    std::cout << "k_first: " << k_first << "\n";
    std::cout << "nk_last: " << nk_last << "\n";
    std::cout << "block_width: " << block_width << "\n";
    std::cout << "block_lines: " << block_lines << "\n";
}   

void IGLupanovSynthesis::scheme_print(std::ostream& ostream, bool additional_info) {
    scheme.print(ostream, additional_info);
}

void IGLupanovSynthesis::MKSprint(std::ostream& ostream) {
    scheme.MKSprint(ostream);
}

void IGLupanovSynthesis::getSchemeFunction(std::ostream& ostream) {
    auto res = scheme.getResult();
    res.print(ostream);
}
