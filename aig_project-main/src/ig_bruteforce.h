#pragma once

#include "ig.h"

extern std::ofstream file;

class IGBruteforcer {
    void bruteforce_outputs(const IG &aig);
    void bruteforce_recursion(const IG &ig, unsigned vertices_left);

    std::vector<bool> function_found;
    unsigned total_amount_function_found;
public:

    IGBruteforcer();
    ~IGBruteforcer();

    void bruteforce(IG ig = IG(), unsigned vertices = 0);
};

