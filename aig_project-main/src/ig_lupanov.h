#pragma once 

#include "ig.h"
#include "function.h"

extern std::ofstream file;

class IGLupanov {
    unsigned n_all_variables;
    unsigned k_first_variables;
    unsigned n_k_last_variables;
    unsigned s_block_width;
    unsigned p_block_count;

    unsigned border_before_cus;
    unsigned border_between_cus;
    unsigned border_start_of_second_cus;
    unsigned border_of_cus;

    Function function;

    IG lupanov_scheme;

    void build_conjuction_universal_set();
    void build_conjuction_universal_set_multi_aig();
public:
    IGLupanov() = default;
    IGLupanov(Function function);

    void synthesis();
    void synthesis_multi_aig();
};