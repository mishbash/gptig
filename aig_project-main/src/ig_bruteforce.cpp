#include "ig_bruteforce.h"

#include <ranges>
#include <string>
#include <algorithm>
#include <fstream>
#include <vector>
#include <cassert>

IGBruteforcer::IGBruteforcer() : function_found(), total_amount_function_found(0) {}

IGBruteforcer::~IGBruteforcer() {}

void IGBruteforcer::bruteforce_outputs(const IG &ig) {
    IG new_ig = ig;
    unsigned max_vertex = ig.getVariableCount() + ig.getNodes().size();
    for (unsigned i = ig.getVariableCount() + 1; i <= max_vertex; ++i) {
        for (unsigned j = 0; j <= 1; ++j) {
            new_ig.setOutput(i);
            new_ig.setNegateOutput(j);
            
            unsigned res = new_ig.getResult().mincode();

            if (!function_found[res]) {
                function_found[res] = 1;
                ++total_amount_function_found;
                new_ig.MKSprint(file);
            }
        }
    }
}


/*
    - pick a function, because different function can have diffrent variable count (1)
    - pick all possible permutations (2)
    - get all possible variants on negativity of input (3)
*/
void IGBruteforcer::bruteforce_recursion(const IG &ig, unsigned vertices_left) {
    if (!vertices_left) {
        bruteforce_outputs(ig);
        return;
    }
    unsigned max_vertex = ig.getVariableCount() + ig.getNodes().size();
    
    auto functions = ig.getFunctions();

    for (auto cur_function : functions) { // (1)
        auto variable_count = cur_function.getVariableCount();

        std::string bitmask(variable_count, 1);
        bitmask.resize(max_vertex, 0);

        do { // (2)
            for (auto output_bitmask : std::views::iota(0u, 1u << variable_count)) { // (3)

                IG new_ig = ig;

                std::vector<unsigned> input_nodes;
                for (auto i : std::views::iota(0u, max_vertex)) {
                    if (bitmask[i]) {
                        input_nodes.push_back(i + 1);
                    }
                }

                std::vector<bool> negate_nodes(variable_count);
                for (auto [rank, node_negation] : std::views::enumerate(negate_nodes)) {
                    node_negation = output_bitmask & (1 << rank);
                }

                new_ig.addNode(IGNode(input_nodes, negate_nodes, cur_function));
                bruteforce_recursion(new_ig, vertices_left - 1);
            }
        } while (std::ranges::prev_permutation(bitmask).found);
    }
}

void IGBruteforcer::bruteforce(IG ig, unsigned vertices) {
    function_found.resize(1 << (1 << (ig.getVariableCount())), false);

    total_amount_function_found = 0;
    bruteforce_recursion(ig, vertices);

    file << (1 << ((1 << ig.getVariableCount()) + 1)) - total_amount_function_found - 1 << "\n";
}

