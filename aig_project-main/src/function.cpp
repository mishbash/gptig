#include "function.h"
#include <cassert>

Function::Function() : variable_count(0), truth_table() {}

Function::Function(unsigned variable_count) : variable_count(variable_count),
    truth_table(1 << variable_count, false) {}

Function::Function(unsigned variable_count, bool value) :
    variable_count(variable_count), truth_table(1 << variable_count, value) {}

Function::Function(const std::vector<bool> &truth_table) :
    variable_count(0), truth_table(truth_table) {
    while ((1 << variable_count) < truth_table.size()) {
        ++variable_count;
    }
}

Function::~Function() {}

bool Function::operator()(const std::vector<bool> &x) const {
    return getValue(x);
}

bool Function::operator==(const Function &function) const {
    return truth_table == function.getTruthTable();
}

bool Function::operator!=(const Function &function) const {
    return truth_table != function.getTruthTable();
}

bool Function::operator<(const Function &function) const {
    for (size_t i = 0; i < truth_table.size(); ++i) {
        if (truth_table[i] < function.getTruthTable()[i]) {
            return true;
        }
        if (truth_table[i] > function.getTruthTable()[i]) {
            return false;
        }
    }
    return false;
}

Function Function::operator~() const {
    std::vector<bool> new_truth_table(truth_table.size());
    for (size_t i = 0; i < truth_table.size(); ++i) {
        new_truth_table[i] = !truth_table[i];
    }
    return Function(new_truth_table);
}

Function Function::operator&(const Function &function) const {
    std::vector<bool> new_truth_table(truth_table.size());
    for (size_t i = 0; i < truth_table.size(); ++i) {
        new_truth_table[i] = truth_table[i] & function.getTruthTable()[i];
    }
    return Function(new_truth_table);
}

unsigned Function::getVariableCount() const {
    return variable_count;
}

const std::vector<bool> &Function::getTruthTable() const {
    return truth_table;
}

bool Function::getValue(const std::vector<bool> &x) const {
    unsigned table_index = 0;
    for (unsigned i = 0; i < x.size(); ++i) {
        table_index = (table_index << 1) + (x[i] ? 1 : 0);
    }
    return truth_table[table_index];
}

void Function::setValue(const std::vector<bool> &x, bool value) {
    unsigned table_index = 0;
    for (unsigned i = 0; i < x.size(); ++i) {
        table_index = (table_index << 1) + (x[i] ? 1 : 0);
    }
    truth_table[table_index] = value;
}

bool Function::isZero() const {
    for (const auto &i : truth_table) {
        if (i) {
            return false;
        }
    }
    return true;
}

void Function::print(std::ostream &ostream, bool next_line) const {
    for (unsigned i = 0; i < truth_table.size(); ++i) {
        ostream << truth_table[i];
    }
    if (next_line) {
        ostream << std::endl;
    }
}

void Function::bigPrint(std::ostream &ostream) const {
    for (unsigned i = 0; i < truth_table.size(); ++i) {

        for (int k = variable_count - 1; k >= 0; --k) {
            bool bit = (i >> k) & 1;
            ostream << bit;
        }

        ostream <<  " " << truth_table[i] << "\n";
    }
    ostream << std::endl;
}

unsigned Function::mincode() const {
    assert(variable_count <= 5);

    unsigned result = 0;
    for (unsigned i = 0; i < truth_table.size(); ++i) {
        result = (result << 1) + (truth_table[i] ? 1 : 0);
    }
    return result;
}

