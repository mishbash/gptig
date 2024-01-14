#include "ig.h"
#include "ig_bruteforce.h"
#include "ig_lupanov.h"

#include <iostream>
#include <string>
#include <fstream>
#include <ranges>

std::ofstream file;

using namespace std;

int main(int argc, char* argv[]) {
    // IG test;

    // test.setVariableCount(5);

    // std::ifstream inputFile("./src/input.txt");
    // if (inputFile.is_open()) {
    //     test.read(inputFile);
    //     inputFile.close();
    // } else {
    //     std::cout << "ERROR OPENING FILE!\n";
    //     return 0;
    // }

    // test.print(std::cout);

    // auto f = test.getResult();
    // f.print(std::cout);
    // // f.bigPrint(std::cout);

    // IG test;
    // test.setVariableCount(2);
    // test.addFunction(Function({0,0,0,1}));
    // IGBruteforcer bruteforce_test;
    // bruteforce_test.bruteforce(test, 2);

    // file.open("output.txt", std::ios::out);

    // IG aig_test;
    // aig_test.setVariableCount(3);
    // aig_test.addFunction(Function({0,0,0,1}));
    // IGBruteforcer bruteforce_aig_test;
    // bruteforce_aig_test.bruteforce(aig_test, 4);


    // IG xaig_test;
    // xaig_test.setVariableCount(4);
    // xaig_test.addFunction(Function({0,0,0,1}));
    // xaig_test.addFunction(Function({0,1,1,0}));
    // IGBruteforcer bruteforce_xaig_test;
    // bruteforce_xaig_test.bruteforce(xaig_test, 5);

    // Function f(5);
    // f.setValue(std::vector<bool> (8, true), true); // f - conjuction;
    // IGLupanov test_lupanov(f);

    if (argc > 2) {
        std::cerr << "WRONG USAGE!\n";
        return 0;
    }

    if (argc == 2) {
        std::string func = argv[1];
        std::vector<bool> func_vector(func.size());
        for (auto [s_val, v_val] : std::views::zip(func, func_vector)) {
            v_val = s_val == '1';
        }
        Function f(func_vector);
        IGLupanov test_lupanov(f);
        test_lupanov.synthesis();
        return 0;
    }

    std::ifstream inputFile("./src/input.txt");
    std::string func;
    inputFile >> func;
    std::vector<bool> func_vector(func.size());
    for (auto [s_val, v_val] : std::views::zip(func, func_vector)) {
        v_val = s_val == '1';
    }
    Function f(func_vector);
    IGLupanov test_lupanov(f);
    test_lupanov.synthesis();

    // IGLupanov test_lupanov(Function({1,0,0,1,0,1,1,1}));
    // IGLupanov test_lupanov(Function({1,0,0,1,0,1,1,1,
    //                                  1,0,1,1,0,0,0,1,
    //                                  1,0,0,0,0,1,0,1,
    //                                  0,0,1,1,0,1,0,1}));

    return 0;
}