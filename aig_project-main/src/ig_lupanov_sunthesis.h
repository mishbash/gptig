#pragma once 

#include "ig.h"
#include "function.h"

extern std::ofstream file;


// Subfunctions info
struct IGLupanovSubFunction {
    std::vector<std::pair<unsigned, bool>> truth_table;
};

struct IGLupanovSubFunctionsList {
    std::vector<IGLupanovSubFunction> sub_functions;
};


class IGLupanovSynthesis {
private:
    // variables
    unsigned variables_count;
    unsigned k_first;
    unsigned nk_last;
    unsigned block_width;
    unsigned block_lines;

    unsigned border_before_cus;
    unsigned border_between_cus;
    unsigned border_start_of_second_cus;
    unsigned border_of_cus;

    // input func and output aig 
    Function function;
    IG scheme;


    // functions
    void build_conjunction_set_naive();
public:
    IGLupanovSynthesis() = default;
    IGLupanovSynthesis(Function function);

    void build();

    void info();
    void scheme_print(std::ostream& ostream, bool additional_info = false);
    void MKSprint(std::ostream& ostream);
};



/*
Algorithm:
- Input function separates on chunks according to block sizes

    |   |   |   | ... | 
----+---+---+---+ ... +
1    [1] [2] [3]  ...  [s]
---
2    [1] [2] [3]  ...  [s]
---
...
---
p    [1] [2] [3]  ...  [s]
-------------------------+



In case of function batches
    |   |       |           | ... | 
----+---+-------+-----------+ ... +
1    [1] [p + 1] [2 * p + 1]  ...  [p * (s - 1) + 1]
---
2    [2] [p + 2] [2 * p + 2]  ...  [p * (s - 1) + 2]
---
...
---
p    [p] [2 * p] [3 * p]      ...  [p * s]
-------------------------+


In one batch is more than one function pieces.
Let's try for just 1-d pieces.

Example:

One chunk as [1] in previous graphic
+---+
|011|
|100|
|111|
|001|
+---+

Converts into three different pieces:


+---------+
|0 | 1 | 1|
|1 | 0 | 0|
|1 | 1 | 1|
|0 | 0 | 1|
+---------+

for all this pieces we should know binary input


In this graphic we separate top-variables

            Normal
              |
              V

top  vars ->        |   |       |           | ... | 
                ----+---+-------+-----------+ ... +
                1    [1] [p + 1] [2 * p + 1]  ...  [p * (s - 1) + 1]
                ---
                2    [2] [p + 2] [2 * p + 2]  ...  [p * (s - 1) + 2]
                ---
                ...
                ---
                p    [p] [2 * p] [3 * p]      ...  [p * s]
                -------------------------+


Top-lever vars needed to calculate info IN chunks, but not for anything else.
We can just separate each and then after calculating all combine some together


For better understanding we will solve one example by hands:


f = 0110'1111'1000'0001
x1 x2 x3 x4 | f
------------+--
 0  0  0  0 | 0
 0  0  0  1 | 1
 0  0  1  0 | 1
 0  0  1  1 | 0

 0  1  0  0 | 1
 0  1  0  1 | 1
 0  1  1  0 | 1
 0  1  1  1 | 1

 1  0  0  0 | 1
 1  0  0  1 | 0
 1  0  1  0 | 0
 1  0  1  1 | 0

 1  1  0  0 | 0
 1  1  0  1 | 0
 1  1  1  0 | 0
 1  1  1  1 | 0


k = floor(log2(n)) = floor(log2(4)) = 2











ALGO:

-- 1 step:
Find subfunctions from function



*/
