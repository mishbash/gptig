#include "ig.h"

#include <ranges>
#include <string>
#include <cassert>
#include <iomanip>

IGNode::IGNode() : children(0), negate(0), function(0) {}

IGNode::IGNode(std::vector<unsigned> children, std::vector<bool> negate, std::vector<bool> values) : 
    children(children), negate(negate), function(values)  {}

IGNode::IGNode(std::vector<unsigned> children, std::vector<bool> negate, Function function) : 
    children(children), negate(negate), function(function)  {}

void IGNode::connect(unsigned child, bool neg) {
    children.push_back(child);
    negate.push_back(neg);
}

IG::IG() : variable_count(4), output(0), negate_output(false), nodes() {}

IG::~IG() {}


unsigned IG::addNode(const IGNode &node) {
    nodes.push_back(node);
    addFunction(node.function);
    return nodes.size() + variable_count;
}

bool IG::hasCycles() const {
    // std::vector<unsigned> used(nodes.size());
    // return traverse_cycles(output, used);
    return false;
}

bool IG::traverse(const std::vector<bool>& x, unsigned v, std::vector<bool>& used, std::vector<bool>& used_res) const {
    assert(v >= 1 && v <= variable_count + nodes.size());

    if ((v >= 1) && (v <= variable_count)) {
        return !x[v - 1];
    }
    v -= variable_count + 1;
    if (used[v]) {
        return used_res[v];
    }
    used[v] = true;

    // check if children != function inputs
    auto node = nodes[v];
    std::vector<bool> t_table(node.children.size(), 0);

    for (int i = 0; auto child : node.children) {
        t_table[i] = traverse(x, child, used, used_res) ^ node.negate[i];
        ++i;
    }

    return used_res[v] = node.function.getValue(t_table);
}


Function IG::getResult() const {
    std::vector<bool> x(variable_count, false);
    Function f(variable_count);
    for (unsigned i = 0; i < (1 << variable_count); ++i) {
        std::vector<bool> used(nodes.size(), 0);
        std::vector<bool> used_res(nodes.size(), 0);
        bool value = traverse(x, output, used, used_res) ^ negate_output;
        f.setValue(x, value);
        for (int j = (variable_count - 1); j >= 0; --j) {
            x[j] = !x[j];
            if (x[j]) {
                break;
            }
        }
    }
    return f;
}


void IG::read(std::istream &istream) {
    nodes.clear();
    unsigned cmplx;
    istream >> cmplx >> output >> negate_output;
    for (unsigned i = 0; i < cmplx; ++i) {
        IGNode node;

        unsigned node_amount;
        istream >> node_amount;

        for (auto k : std::views::iota(0u, node_amount)) {
            unsigned new_node;
            bool new_node_negate;
            istream >> new_node >> new_node_negate;
            node.connect(new_node, new_node_negate);
        }

        std::vector<bool> new_node_function(1 << node_amount);
        std::string s;
        istream >> s;

        for (auto [index, value] : std::views::enumerate(new_node_function)) {
            value = s[index] == '1';
        }

        node.function = Function(new_node_function);

        nodes.push_back(node);
    }
}

void IG::print(std::ostream &ostream, bool additional_info) const {
    ostream << getComplexity() << "\n";
    ostream << output << " " << (negate_output ? 1 : 0) << "\n\n";

    int count = variable_count + 1;
    for (auto& ig_node : nodes) {
        ostream << "Node #" << count << "\n";
        if (additional_info) ostream << "Connected:\n";
        for (const auto& c : ig_node.children) {
            ostream << c << " ";
        }
        ostream << "\n";
        if (additional_info) ostream << "Negate:\n";
        for (const auto& n : ig_node.negate) {
            ostream << n << " ";
        }
        ostream << "\n";
        if (additional_info) ostream << "Function\n";
        ig_node.function.print(ostream);
        ostream << "\n";
        ++count;
    }
}

void IG::MKSprint(std::ostream& ostream) const {
    auto result = getResult();

    // ostream << std::setw(getVariableCount()) << result.mincode() << " ";
    ostream << getComplexity() << " ";
    ostream << output << " " << negate_output << " ";

    for (auto& ig_node : nodes) {
        assert(ig_node.children.size() == 2);
        assert(ig_node.negate.size() == 2);
        ostream << ig_node.children[0] << " " << ig_node.negate[0] << " ";
        ostream << ig_node.children[1] << " " << ig_node.negate[1] << " ";

    }
    ostream << std::endl;
}

unsigned IG::getVariableCount() const {
    return variable_count;
}

void IG::setVariableCount(unsigned count) {
    variable_count = count;
}

unsigned IG::getOutput() const {
    return output;
}

void IG::setOutput(unsigned vertex) {
    output = vertex;
}

bool IG::getNegateOutput() const {
    return negate_output;
}

void IG::setNegateOutput(bool value) {
    negate_output = value;
}

const unsigned IG::getLastNodeNum() const {
    return nodes.size() + variable_count;
}

const IGNode IG::getSpecificNode(unsigned num) const {
    assert(num < nodes.size());
    return nodes[num];
}

std::vector<IGNode> &IG::getNodes() {
    return nodes;
}

const std::vector<IGNode> &IG::getNodes() const {
    return nodes;
}

unsigned IG::getComplexity() const {
    return nodes.size();
}

std::vector<Function>& IG::getFunctions() {
    return functions;
}

const std::vector<Function>& IG::getFunctions() const {
    return functions;
}

bool IG::addFunction(Function new_function) {
    // std::cerr << "CHECK\n";
    // new_function.print(std::cerr);
    
    for (auto i : functions) {
        // std::cerr << "EXISTING FUNC TO CHECK\n";
        // i.print(std::cerr);
        // std::cerr << "\n";

        if (i.getVariableCount() != new_function.getVariableCount()) {
            // std::cerr << "GO NEXT NOT SAME SIZE" << "\n";
            continue; // not same size
        }

        if (i.getVariableCount() <= 5) { // if same size and can be checked by mincode
            if (new_function.mincode() == i.mincode()) {
                // std::cerr << "SAME MINCODE\n";
                return false;
            }
        } else {

            // no mincode - by the value

            bool flag = true;
            for (auto [i_val, new_val] : std::views::zip(i.getTruthTable(), new_function.getTruthTable())) {
                if (flag && i_val != new_val) {
                    flag = false;
                }
            }

            if (flag) {
                // std::cerr << "SAME FUNC\n";
                return false;
            }
        }
    }

    // std::cerr << "NEW FUNC\n";
    functions.push_back(new_function);
    return true;
}



/*



[1][2][3][][][][][][][][][][]


      [1]
      /\
   [2] [3]
  /\    /\
[4][5][6][7]




       [1]
      /  \
   [2]   [3]
        / | \
      [4][5][6]




*/