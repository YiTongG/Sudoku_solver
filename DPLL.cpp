#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <fstream>

using namespace std;
using Clause = vector<string>;
using Formula = vector<Clause>;
using Assignment = map<string, bool>;

Formula applyAssignmentToFormula(const Formula &formula, const string &var, bool val) {
    Formula newFormula;
    for (const Clause &clause: formula) {
        bool skipClause = false;
        Clause newClause;
        for (const string &literal: clause) {
            string literalVar = literal[0] != '!' ? literal : literal.substr(1);
            if (literalVar == var) {
                // 如果文字被评估为真
                if ((literal[0] != '!' && val) || (literal[0] == '!' && !val)) {
                    // 整个子句也被评估为真，所以我们跳过整个子句
                    skipClause = true;
                    break;
                }
                // 对于被评估为假的文字，仅删除文字
                continue;
            }
            newClause.push_back(literal);
        }
        if (!skipClause) {
            newFormula.push_back(newClause);
        }
    }
    return newFormula;
}


string getVariableForSplit(const Formula &formula) {
    if (!formula.empty() && !formula[0].empty()) {
        string literal = formula[0][0];
        return literal[0] != '!' ? literal : literal.substr(1);
    }
    return "";
}

Assignment handleUnitClauses(const Formula &formula, const Assignment &assignments) {
    Assignment newAssignments = assignments;
    bool changesMade = false;

    for (size_t i = 0; i < formula.size(); i++) {
        if (formula[i].size() == 1) {
            string literal = formula[i][0];
            bool val = literal[0] != '!';
            string var = val ? literal : literal.substr(1);

            if (assignments.find(var) == assignments.end()) {
                newAssignments[var] = val;
                changesMade = true;
            } else if (assignments.at(var) != val) {
                return {};  // Conflict detected
            }
        }
    }

    return changesMade ? newAssignments : assignments;
}

Formula applyMultipleAssignmentsToFormula(const Formula &formula, const Assignment &assignments) {
    Formula updatedFormula = formula;
    for (const auto &[var, val]: assignments) {
        updatedFormula = applyAssignmentToFormula(updatedFormula, var, val);
    }
    return updatedFormula;
}

Assignment dpll(const Formula &formula, const Assignment &initialAssignments) {
    // Base cases
    if (formula.empty()) {
        return initialAssignments;  // Satisfiable with current assignments
    }

    if (any_of(formula.begin(), formula.end(), [](const Clause &clause) { return clause.empty(); })) {
        return {};  // Empty clause found, unsatisfiable
    }

    // Apply unit clause rule
    for (size_t i = 0; i < formula.size(); i++) {
        if (formula[i].size() == 1) {
            string literal = formula[i][0];
            bool val;
            string var;
            if (literal[0] == '!') {
                val = false;   // This is a negation, so we assign the opposite
                var = literal.substr(1);
            } else {
                val = true;
                var = literal;
            }


            Assignment newAssignments = initialAssignments;
            newAssignments[var] = val;

            // Update the formula according to the assignment
            Formula updatedFormula = applyAssignmentToFormula(formula, var, val);
            return dpll(updatedFormula, newAssignments);
        }
    }

    // Apply split rule
    string splitVar = getVariableForSplit(formula);
    if (initialAssignments.find(splitVar) != initialAssignments.end()) {
        return {};  // We shouldn't split on a variable that already has an initial value
    }

    Assignment trueAssignment = initialAssignments;
    trueAssignment[splitVar] = true;

    Formula trueFormula = applyAssignmentToFormula(formula, splitVar, true);
    Assignment result = dpll(trueFormula, trueAssignment);

    if (!result.empty()) {
        return result;
    }

    Assignment falseAssignment = initialAssignments;
    falseAssignment[splitVar] = false;
    Formula falseFormula = applyAssignmentToFormula(formula, splitVar, false);
    return dpll(falseFormula, falseAssignment);
}

