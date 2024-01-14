#pragma once

#include <vector>
#include <iostream>

class Function {
    unsigned variable_count;
    std::vector<bool> truth_table;
public:

    Function();
    Function(unsigned variable_count);
    Function(unsigned variable_count, bool value);
    Function(const std::vector<bool> &truth_table);

    ~Function();

    bool operator()(const std::vector<bool> &x) const;
    bool operator==(const Function &function) const;
    bool operator!=(const Function &function) const;
    bool operator<(const Function &function) const;

    Function operator~() const;
    Function operator&(const Function &function) const;

    unsigned getVariableCount() const;
    const std::vector<bool> &getTruthTable() const;
    void setValue(const std::vector<bool> &x, bool value);
    bool getValue(const std::vector<bool> &x) const;

    unsigned mincode() const;

    bool isZero() const;

    void print(std::ostream &ostream, bool next_line = true) const;
    void bigPrint(std::ostream &ostream) const;
};
