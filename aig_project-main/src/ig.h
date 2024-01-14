#pragma once

#include "function.h"

#include <vector>
#include <iostream>

struct IGNode {
    std::vector<unsigned> children;
    std::vector<bool> negate;

    Function function;

    void connect(unsigned child, bool neg);

    IGNode();
    IGNode(std::vector<unsigned> children, std::vector<bool> negate, std::vector<bool> values);
    IGNode(std::vector<unsigned> children, std::vector<bool> negate, Function function);
};

class IG {
public:
    IG();
    ~IG();

    unsigned getVariableCount() const;
    void setVariableCount(unsigned count);

    unsigned getOutput() const;
    void setOutput(unsigned vertex);
    
    bool getNegateOutput() const;
    void setNegateOutput(bool value);

    std::vector<Function>& getFunctions();
    const std::vector<Function>& getFunctions() const;
    bool addFunction(Function new_function);

    const unsigned getLastNodeNum() const;
    const IGNode getSpecificNode(unsigned num) const;
    std::vector<IGNode>& getNodes();
    const std::vector<IGNode>& getNodes() const;
    unsigned addNode(const IGNode &node);
    
    bool hasCycles() const;

    unsigned getComplexity() const;
    Function getResult() const;

    void read(std::istream &istream);
    void print(std::ostream& ostream, bool additional_info = false) const;
    void MKSprint(std::ostream& ostream) const;

private:
    unsigned variable_count;
    unsigned output;
    bool negate_output;
    std::vector<Function> functions;
    std::vector<IGNode> nodes;

    bool traverse(const std::vector<bool>& x, unsigned v, std::vector<bool>& used, std::vector<bool>& used_res) const;
};

