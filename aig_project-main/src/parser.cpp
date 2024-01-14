#include <iostream>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <stack>
#include <vector>
#include <string>
#include <functional>

#include "parser.h"

using std::cout;
using std::cerr;
using std::string;
using std::stack;
using std::vector;

// Функция для проверки, является ли символ оператором
bool Parser::isOperator(char c) const {
    return c == '&' || c == '|' || c == '^';
}


// Функция для проверки, является ли символ буквой переменной или константой
bool Parser::isOperand(char c) const {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c =='1') || (c == '0');
}
 

// Функция для определения приоритета оператора
int Parser::getPriority(char c) const {
    if (c == '&') { return 3; }
    if (c == '^') { return 2; } 
    if (c == '|') { return 1; }
    return 0;
}

// Функция для преобразования функции в ПОЛИЗ
string Parser::toPolish(string function) {
    string result;
    stack<char> opStack;
 
    if (!InputChecker(function)) { return {""}; }

    for (int i = 0; i < function.length(); i++) {
        // Если символ является переменной, добавляем ее в выходную строку
        if (isOperand(function[i])) {
            result.push_back(function[i]);
        }
            // Если символ является оператором
        else if (isOperator(function[i])) {
            // Если стек операторов не пуст и текущий оператор имеет меньший приоритет, чем оператор на вершине стека,
            // выталкиваем операторы из стека в выходную строку, пока это возможно
            while (!opStack.empty() && getPriority(opStack.top()) >= getPriority(function[i])) {
                result.push_back(opStack.top());
                opStack.pop();
            }
            // Добавляем текущий оператор в стек операторов
            opStack.push(function[i]);
        }
            // Если символ - открывающая скобка, добавляем ее в стек операторов
        else if (function[i] == '(') {
            opStack.push(function[i]);
        }
            // Если символ - закрывающая скобка, выталкиваем операторы из стека в выходную строку до открывающей скобки,
            // после чего удаляем открывающую скобку из стека
        else if (function[i] == ')') {
            while (!opStack.empty() && opStack.top() != '(') {
                result.push_back(opStack.top());
                opStack.pop();
            }
            if (!opStack.empty() && opStack.top() == '(') {
                opStack.pop();
            }
        }
    }
 
    // Выталкиваем оставшиеся операторы из стека в выходную строку
    while (!opStack.empty()) {
        result.push_back(opStack.top());
        opStack.pop();
    }
 
    return result;
}



void Parser::run() { 
    std::ifstream input(input_file_);
    std::ofstream output(output_file_);
 
    if (!input) {
        cerr << "Невозможно открыть входной файл!\n";
        return;
    }
 
    if (!output) {
        cerr << "Невозможно открыть выходной файл\n";
        return;
    }
 
    vector<string> functions;
    string line;
 
    // Считываем функции из файла
    while (std::getline(input, line)) {
        functions.push_back(line);
    }
 
    // Представляем функции в виде ПОЛИЗ в массиве строк
    for (int i = 0; i < functions.size(); i++) {
        string poliz = toPolish(functions[i]);
        output << "Функция №" << i + 1 << " в виде ПОЛИЗ: ";
        output << poliz << "\n";
        output << "\n";
    }









    input.close();
    output.close();
}

void Parser::test_run() {
    std::ifstream input(input_file_);
 
    if (!input) {
        cerr << "Невозможно открыть входной файл для тестирования!\n";
        return;
    }


    vector<vector<string>> test_case = {
        {
            "&",
            "|",
        },

        {
            "x | x",
            "x & y",
            "x & 1",
            "z ^ 0",
            "q",
            "1",
            "0" 
        },

        {
            "x & x & x & x & x & x & x",
            "x | x | x | x | | | | | | | | | |",
            "::::::",
            ")))(((())))",
            "(((((())))))"
        }
    };

    int test_count = 1;
    for (auto i : test_case) {
        cout << "test #" << test_count << "\n";
        ++test_count;

        for (auto j : i) {
            auto prog_ans = toPolish(j);

            if (prog_ans == "") continue;

            cout << "string: " << j << "\n";
            cout << "output: ";

            cout << prog_ans << "\n";

            cout << "\n\n";
        }
        
        cout << "\n";

    }

    input.close();
}

void Parser::test_calc() {

    std::ifstream input(input_file_);
    std::map<char, bool> m;

    if (!input) {
        cerr << "Невозможно открыть входной файл для тестирования!\n";
        return;
    }


    vector<string> test_case = {
        "0 | 0",
        "0 | 1",
        "1 | 0",
        "1 | 1",

        "0 ^ 0",
        "0 ^ 1",
        "1 ^ 0",
        "1 ^ 1",

        "0 & 0",
        "0 & 1",
        "1 & 0",
        "1 & 1",

        
        "x | x",
        "x & y",
        "x & 1",
        "z ^ 0",
        "1",
        "0" 

    };

    int test_count = 1;
    for (auto i : test_case) {
        cout << "test #" << test_count << "\n";
        ++test_count;
        string s_calc = i;
        s_calc = toPolish(s_calc);
        for(auto c: s_calc){
            m[c] = 1;
        }
        m['0'] = 0;
        m['1'] = 1;
        cout << s_calc << "\n";
        cout << "res: " << Calculate(s_calc, m) << "\n\n";
        fflush(stdout);
    }

    input.close();
}


/*
class should prepare input for parser
Only allowed lower case letters except j... and J... and mb more
But for now only available lower case and common operators

- all lower case
- delete all spaces
- if string consists of bad symbols - return Error "Wrong Usage"
- bracket check
- Reverse polish notaion. If Reversed == Initial - all good. Else
*/


bool Parser::InputChecker(std::string& s) {
    auto s_oper = s;

    s.erase(remove(s.begin(), s.end(), ' '), s.end());

    for (auto c : s) {
        if (!(isOperand(c) || isOperator(c) || c == '(' || c == ')')) {
            cerr << "FOUND WRONG SYMBOL : " << c << "\n";
            return false;
        }
    }

    int open_bracket = 0;

    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) -> unsigned char {
        return tolower(c);
    });

    for (auto c : s) {
        if (c == '(') ++open_bracket;
        if (c == ')') {
            if (open_bracket > 0) {
                --open_bracket;
            } else {
                cerr << "FOUND WRONG BRACKET SEQUENCE\n";
                return false;
            }
        }
    }

    int operand_count = 0, operator_count = 0;
    
    for (auto c : s) {
        if (isOperand(c)) ++operand_count;
        if (isOperator(c)) ++operator_count;
    }

    if (operand_count != operator_count + 1 && !(operator_count == 0 && operand_count == 1)) {
        cerr << "FOUND WRONG AMOUNT OF OPERATORS AND OPERANDS\n";
        return false;
    }

    // now string is good

    transform(s_oper.begin(), s_oper.end(), s_oper.begin(), [](unsigned char c) -> unsigned char {
        if (isalpha(c)) {
            return tolower(c);
        } else {
            return ' ';
        }
    });

    return true;
}



bool Parser::Calculate(string& s, std::map<char, bool>& m) {
    stack<bool> var_stack;

    for (auto c : s) {
        if (isOperand(c))  {
            var_stack.push(m[c]);
        } else {
            auto a = var_stack.top();
            var_stack.pop();
            auto b = var_stack.top();
            var_stack.pop();

            auto res = oper_map[c](a, b);
            var_stack.push(res);
        }
    }

    return var_stack.top();
}
