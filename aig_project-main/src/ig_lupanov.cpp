#include "ig_lupanov.h"

#include <vector>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <cassert>

IGLupanov::IGLupanov(Function function) : function(function) {
    n_all_variables = function.getVariableCount();

    k_first_variables = 0; // log2(n)
    while ((1 << (k_first_variables + 1)) < n_all_variables) {
        ++k_first_variables;
    }

    k_first_variables = n_all_variables - k_first_variables;
    n_k_last_variables = n_all_variables - k_first_variables;
    // s_block_width = n_all_variables - 3 * (k_first_variables / 2); // n - 3log2(n)
    // p_block_count = (n_all_variables + s_block_width - 1) / s_block_width; // ceil(n / s)
}


void IGLupanov::build_conjuction_universal_set_multi_aig() {
    lupanov_scheme.setVariableCount(n_all_variables);
    assert(k_first_variables >= 1);

    Function f_k(k_first_variables);
    f_k.setValue(std::vector<bool>(k_first_variables, true), true); // f - conjuction;
    lupanov_scheme.addFunction(f_k);

    Function f_n_k(n_k_last_variables);
    f_n_k.setValue(std::vector<bool>(n_k_last_variables, true), true); // f - conjuction;
    lupanov_scheme.addFunction(f_n_k);


    std::vector<unsigned> k_first_nodes_array(k_first_variables);
    std::ranges::iota(k_first_nodes_array, 1);
    std::vector<unsigned> n_k_last_nodes_array(n_k_last_variables);
    std::ranges::iota(n_k_last_nodes_array, k_first_variables + 1);

    for (auto bitmask : std::views::iota(0u, 1u << k_first_variables)) { 
        std::vector<bool> negate_nodes(k_first_variables, 0);
        for (auto [rank, node_negation] : std::views::enumerate(negate_nodes)) {
            node_negation = bitmask & (1 << rank);
        }

        border_between_cus = lupanov_scheme.addNode(IGNode(k_first_nodes_array, negate_nodes, f_k));
    }

    for (auto bitmask : std::views::iota(0u, 1u << n_k_last_variables)) { 
        std::vector<bool> negate_nodes(n_k_last_variables, 0);
        for (auto [rank, node_negation] : std::views::enumerate(negate_nodes)) {
            node_negation = bitmask & (1 << rank);
        }

        border_of_cus = lupanov_scheme.addNode(IGNode(n_k_last_nodes_array, negate_nodes, f_n_k));
    }
}


void IGLupanov::synthesis_multi_aig() {
    build_conjuction_universal_set();

    std::vector<unsigned> nodes_to_output;
    for (auto n_k_bitmask : std::views::iota(0u, 1u << n_k_last_variables)) { 
        std::vector<bool> binary_set(n_all_variables, 0);
        for (auto i : std::views::iota(0u, n_k_last_variables)) {
            binary_set[i + k_first_variables] = n_k_bitmask & (1 << i);
        }

        IGNode node;
        for (auto k_bitmask : std::views::iota(0u, 1u << k_first_variables)) { 
            for (auto i : std::views::iota(0u, k_first_variables)) {
                binary_set[i] = k_bitmask & (1 << i);
            }   
            
            // std::cout << "CHECK\n";
            // Function(binary_set).print(std::cout);
            if (auto res = function.getValue(binary_set)) {
                // std::cout << "node added\n";
                node.connect(n_all_variables + k_bitmask + 1, 0);
            }
        }  

        Function f(node.children.size(), true);
        f.setValue(std::vector<bool>(node.children.size(), false), false); // f - disjunction;
        node.function = f;

        if (node.children.size() > 0) {
            auto last_node_num = lupanov_scheme.addNode(node);
            auto lnode = lupanov_scheme.addNode(IGNode({last_node_num, border_between_cus + n_k_bitmask + 1}, {0, 0}, {0, 0, 0, 1}));
            nodes_to_output.push_back(lnode);
        }

    }

    IGNode node;
    Function f(nodes_to_output.size(), true);
    f.setValue(std::vector<bool>(nodes_to_output.size(), false), false); // f - disjunction;
    node.function = f;
    auto output_num = lupanov_scheme.addNode(IGNode(nodes_to_output, 
                                                    std::vector<bool>(nodes_to_output.size(), false),
                                                    f));
    lupanov_scheme.setOutput(output_num);

    function.print(std::cout);
    auto res = lupanov_scheme.getResult();
    res.print(std::cout);

}


void IGLupanov::build_conjuction_universal_set() {
    lupanov_scheme.setVariableCount(n_all_variables);
    assert(k_first_variables >= 1);

    Function Conjunction({0,0,0,1});

    auto start_period = lupanov_scheme.getLastNodeNum();

                        lupanov_scheme.addNode(IGNode({1, 1}, {0, 0}, Conjunction));
    auto stop_period =  lupanov_scheme.addNode(IGNode({1, 1}, {1, 1}, Conjunction));

    for (auto cur_input : std::views::iota(2u, k_first_variables + 1)) {
        for (auto binary_set : std::views::iota(start_period + 1, stop_period + 1)) {
            lupanov_scheme.addNode(IGNode({binary_set, cur_input}, {0, 0}, Conjunction));
        }
        for (auto binary_set : std::views::iota(start_period + 1, stop_period + 1)) {
            lupanov_scheme.addNode(IGNode({binary_set, cur_input}, {0, 1}, Conjunction));
        }

        start_period = stop_period;
        stop_period = lupanov_scheme.getLastNodeNum();
    }

    border_before_cus = start_period + 1;
    border_between_cus = stop_period;

    start_period = lupanov_scheme.getLastNodeNum();

                    lupanov_scheme.addNode(IGNode({k_first_variables + 1, k_first_variables + 1}, {0, 0}, Conjunction));
    stop_period =   lupanov_scheme.addNode(IGNode({k_first_variables + 1, k_first_variables + 1}, {1, 1}, Conjunction));

    for (auto cur_input : std::views::iota(k_first_variables + 2, n_all_variables + 1)) {
        for (auto binary_set : std::views::iota(start_period + 1, stop_period + 1)) {
            lupanov_scheme.addNode(IGNode({binary_set, cur_input}, {0, 0}, Conjunction));
        }
        for (auto binary_set : std::views::iota(start_period + 1, stop_period + 1)) {
            lupanov_scheme.addNode(IGNode({binary_set, cur_input}, {0, 1}, Conjunction));
        }

        start_period = stop_period;
        stop_period = lupanov_scheme.getLastNodeNum();
    }

    border_start_of_second_cus = start_period + 1;
    border_of_cus = stop_period;


    lupanov_scheme.print(std::cout);
    std::cout << "First part:  " << border_before_cus << " " << border_between_cus << "\n";
    std::cout << "Second part: " << border_start_of_second_cus << " " << border_of_cus << "\n";
}


/*

    f(x1, x2, x3, x4) = f(0, 0, x3, x4) V f(0, 1, x3, x4) V f(1, 0, x3, x4) V f(1, 1, x3, x4)

    -> f(i, j, x3, x4) synthesis i = 0..1, j = 0..1 

*/


void IGLupanov::synthesis() {
    build_conjuction_universal_set();

    Function Conjunction({0,0,0,1});
    // Function Disjunction({0,1,1,1});

    // for (auto k_bitmask: std:views::iota(0u, 1u << k_first_variables)) {
    //     std::vector<bool> binary_set;
    //     for (auto n_k_bitmask: std::views::iota(0u, 1u << n_k_last_variables)) {
            
    //     }


    // }


    unsigned last_node_to_output = -1;
    for (auto n_k_bitmask : std::views::iota(0u, 1u << n_k_last_variables)) { 
        std::vector<bool> binary_set(n_all_variables, 0);
        for (auto i : std::views::iota(0u, n_k_last_variables)) {
            binary_set[i + k_first_variables] = n_k_bitmask & (1 << i);
        }


        // form a single CNF for one binary_set
        unsigned prev_node_to_connect = -1;
        for (auto k_bitmask : std::views::iota(0u, 1u << k_first_variables)) { 
            for (auto i : std::views::iota(0u, k_first_variables)) {
                binary_set[i] = k_bitmask & (1 << i);
            }   
            
            // std::cout << "CHECK\n";
            // Function(binary_set).print(std::cout);
            if (auto res = function.getValue(binary_set)) {
                // std::cout << "node added\n";
                auto node_num = border_before_cus + k_bitmask;
                if (prev_node_to_connect == -1) {
                    prev_node_to_connect = lupanov_scheme.addNode(IGNode({node_num, node_num}, {0, 0}, Conjunction));
                } else {
                    // negate input and mark output as negated
                    prev_node_to_connect = lupanov_scheme.addNode(IGNode({prev_node_to_connect, node_num}, {1, 1}, Conjunction));
                    prev_node_to_connect = lupanov_scheme.addNode(IGNode({prev_node_to_connect, prev_node_to_connect}, {1, 1}, Conjunction));
                }

            }
        }  

        if (prev_node_to_connect == -1) continue;

        // now we get a single disjunction part
        prev_node_to_connect = lupanov_scheme.addNode(IGNode({prev_node_to_connect, border_start_of_second_cus + n_k_bitmask}, {0, 0}, Conjunction));

        if (last_node_to_output == -1) {
            last_node_to_output = lupanov_scheme.addNode(IGNode({prev_node_to_connect, prev_node_to_connect}, {0, 0}, Conjunction));
        } else {
            last_node_to_output = lupanov_scheme.addNode(IGNode({last_node_to_output, prev_node_to_connect}, {1, 1}, Conjunction));
            last_node_to_output = lupanov_scheme.addNode(IGNode({last_node_to_output, last_node_to_output}, {1, 1}, Conjunction));
        }
    }

    lupanov_scheme.setOutput(lupanov_scheme.getLastNodeNum());
    // lupanov_scheme.print(std::cout);
    lupanov_scheme.MKSprint(std::cout);

    // function.print(std::cout);
    // auto res = lupanov_scheme.getResult();
    // res.print(std::cout);
}
