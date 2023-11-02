//
// Created by yitong on 2023/10/25.
//

#ifndef DPLL_H
#define DPLL_H

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <stack>
using Clause = vector<string>;
using Formula = vector<Clause>;
using Assignment = map<string, bool>;

using namespace std;


Assignment dpll(const Formula &formula, const Assignment &assignments);
#endif //DPLL_H
