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

void IGLupanovSynthesis::build() {
    scheme.setVariableCount(variables_count);
    build_conjunction_set_naive();

    // std::vector<IGLupanovSubFunctionsList> sub_func_list;

    // for (auto bitmask : std::views::iota(0u, 1u << )) { 
    // }


    info();

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
