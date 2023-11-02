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
// Check if the clause is a unit clause.
bool isUnit(const std::vector<std::string>& clause, const std::map<std::string, bool>& assignments);

// Check if the literal is a pure literal.
bool isPure(const std::string& literal, const std::vector<std::vector<std::string>>& clauses);

// Reduce the CNF based on the current assignment.
std::vector<std::vector<std::string>> reduceClauses(const std::vector<std::vector<std::string>>& clauses, const std::string& literal);

// The DPLL algorithm implementation.
set<string> initializeLockedAssignments(const vector<vector<string>>& clauses);

Assignment dpll(const Formula &formula, const Assignment &assignments);
map<string, bool> dpllRecursive(vector<vector<string>> clauses, map<string, bool> assignments);
#endif //DPLL_H
