#pragma once

#include <map>
#include <functional>

class Parser {
public:
    void run();
    void test_run();
    void test_calc();

private:
    bool isOperator(char c) const;
    bool isOperand(char c) const;
    int getPriority(char c) const;
    bool InputChecker(std::string& s);
    bool Calculate(std::string& s, std::map<char, bool>& m);

    std::string toPolish(std::string function);

    std::string input_file_ = "./src/input.txt";
    std::string output_file_ = "output.txt";

    std::map<char, std::function<bool(bool, bool)>> oper_map = {
        {'&', [](bool a, bool b) -> bool { return a & b; }},
        {'|', [](bool a, bool b) -> bool { return a | b; }},
        {'^', [](bool a, bool b) -> bool { return a ^ b; }}
    };
};