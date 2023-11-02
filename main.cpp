//
// Created by yitong on 2023/10/25.
//
#include <fstream>
#include <iostream>
#include "SudokuBoard.h"
#include "DPLL.h"
#include <vector>
#include <string>
#include"CNFConverter.h"

using namespace std;

string assign(int num, int row, int col) {
    return "n" + to_string(num) + "_r" + to_string(row) + "_c" + to_string(col);
}

vector<string> sudokuConstraints(const SudokuBoard &board) {
    vector<string> clauses;

    for (int row = 1; row <= 9; ++row) {
        for (int col = 1; col <= 9; ++col) {
            // 1) At least one digit in a box
            string clause1 = "";
            for (int num = 1; num <= 9; ++num) {
                clause1 += assign(num, row, col) + " v ";
            }
            clause1.pop_back(); // remove the last space
            clause1.pop_back(); // remove the last 'v'
            clauses.push_back(clause1);

            for (int num = 1; num <= 9; ++num) {
                // 2) Unique row
                string clause2 = assign(num, row, col) + " => !(";
                for (int other_col = 1; other_col <= 9; ++other_col) {
                    if (other_col != col) {
                        clause2 += assign(num, row, other_col) + " v ";
                    }
                }
                clause2.pop_back(); // remove the last space
                clause2.pop_back(); // remove the last 'v'
                clause2 += ")";
                clauses.push_back(clause2);

                // 3) Unique column
                string clause3 = assign(num, row, col) + " => !(";
                for (int other_row = 1; other_row <= 9; ++other_row) {
                    if (other_row != row) {
                        clause3 += assign(num, other_row, col) + " v ";
                    }
                }
                clause3.pop_back(); // remove the last space
                clause3.pop_back(); // remove the last 'v'
                clause3 += ")";
                clauses.push_back(clause3);

                // 4) Unique 3x3
                string clause4 = assign(num, row, col) + " => !(";
                int startRow = (row - 1) / 3 * 3 + 1;
                int startCol = (col - 1) / 3 * 3 + 1;
                for (int r = startRow; r < startRow + 3; ++r) {
                    for (int c = startCol; c < startCol + 3; ++c) {
                        if (r != row || c != col) {
                            clause4 += assign(num, r, c) + " v ";
                        }
                    }
                }
                clause4.pop_back(); // remove the last space
                clause4.pop_back(); // remove the last 'v'
                clause4 += ")";
                clauses.push_back(clause4);
            }
        }
    }

    // 5) Initial board
    for (int row = 1; row <= 9; ++row) {
        for (int col = 1; col <= 9; ++col) {
            int cellValue = board.getCell(row, col); // Assuming SudokuBoard has getCell method
            if (cellValue) {
                clauses.push_back(assign(cellValue, row, col));
            }
        }
    }

    return clauses;
}


vector<vector<string>> convertToDPLLInput(const vector<string> &clauses) {
    vector<vector<string>> dpllInput;

    for (const auto &clauseStr: clauses) {
        vector<string> clause;

        size_t pos = 0;  // 当前位置
        while (pos < clauseStr.size()) {
            // 跳过前导空格
            while (pos < clauseStr.size() && isspace(clauseStr[pos])) {
                ++pos;
            }

            // 找到单词的结束位置
            size_t end = pos;
            while (end < clauseStr.size() && !isspace(clauseStr[end])) {
                ++end;
            }

            // 如果找到了单词，添加到子句中
            if (end > pos) {
                clause.push_back(clauseStr.substr(pos, end - pos));
            }

            pos = end;
        }

        dpllInput.push_back(clause);
    }

    return dpllInput;
}

void writeAssignmentsToFile(const map<string, bool> &assignments, const string &filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }

    for (const auto &pair: assignments) {
        string literal = pair.first;
        bool value = pair.second;

        // 检查文字是否以!开头，并相应地调整值
        if (literal[0] == '!') {
            literal = literal.substr(1);
            value = !value;
        }

        outFile << literal << " = ";
        if (value) {
            outFile << "true";
        } else {
            outFile << "false";
        }
        outFile << endl;
    }
    outFile.close();
}

bool isValidSudokuInput(const std::vector<std::string> &inputs) {
    for (const std::string &input : inputs) {

        int row = input[0] - '0';
        int col = input[1] - '0';
        int val = input[3] - '0';

        if (input[2] != '=' || row < 1 || row > 9 || col < 1 || col > 9 || val < 1 || val > 9) {
            return false;
        }
    }
    return true;
}

// Function to write CNF clauses to a file
bool writeCnfToFile(const std::vector<std::string> &cnfClauses, const std::string &filename) {
    std::ofstream outFile(filename);  // Attempt to open the file

    if (!outFile.is_open()) {  // Check if the file was opened successfully
        std::cerr << "Unable to open file " << filename << " for writing." << std::endl;
        return false;  // Return false if the file couldn't be opened
    }

    for (const std::string &clause: cnfClauses) {  // Iterate through the CNF clauses
        outFile << clause << std::endl;  // Write each clause followed by a newline
    }

    outFile.close();  // Close the file stream
    return true;  // Return true if everything was written successfully
}

int main(int argc, char *argv[]) {
    bool verboseMode = false;
    bool sudokuMode = true;  // Default mode is Sudoku
    string filename;
    bool bnfMode = false;
    std::vector<std::string> sudokuInputs; // To store Sudoku inputs if provided

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-v") {
            verboseMode = true;
        } else if (arg == "-bnf" && i + 1 < argc) {
            sudokuMode = false;
            bnfMode = true;
            filename = argv[++i];
            i++; // Skip the next argument since it's the filename
        } else {
            // If not in BNF mode, treat it as Sudoku input
            if (!bnfMode) {
                sudokuInputs.push_back(arg);
            } else {
                std::cerr << "Unexpected argument after -bnf flag: " << arg << std::endl;
                return 1;
            }
        }
    }


    if (sudokuMode) {
        if (!isValidSudokuInput(sudokuInputs)) {
            std::cerr << "Invalid input format or values out of range." << std::endl;
            return 1;
        }
        SudokuBoard board;

        // Reading from command line and populating board
        for (const auto &input: sudokuInputs) {
            int row = input[0] - '1';
            int col = input[1] - '1';
            int val = input[3] - '0';
            board.setCell(row, col, val);
        }

        vector<string> clauses = sudokuConstraints(board);

        CNFConverter converter;
        vector<string> cnfClauses = converter.convert(clauses);

        if (verboseMode) { //verboseMode to write cnfforsudoku to file
            // Call the function to write CNF clauses to a file
            if (!writeCnfToFile(cnfClauses, "cnfForSudoku1.txt")) {
                return 1;  // If writing to the file failed, return an error code
            }
        }


        vector<vector<string>> inputForDPLL = convertToDPLLInput(cnfClauses);


        map<string, bool> initialAssignments;

        map<string, bool> assignments = dpll(inputForDPLL, initialAssignments);

        if (verboseMode) {
            writeAssignmentsToFile(assignments, "dp_output.txt");
        }
        if (assignments.empty()) {
            cout << "No solution found!\n";
        } else {
            cout << "Sudoku Solution:\n";
            // Extract solution from the assignments and display
            for (int r = 1; r <= 9; ++r) {
                for (int c = 1; c <= 9; ++c) {
                    for (int n = 1; n <= 9; ++n) {
                        string var = "n" + to_string(n) + "_r" + to_string(r) + "_c" + to_string(c);
                        if (assignments[var]) {
                            cout << n << " ";
                            break;
                        }
                    }
                }
                cout << "\n";
            }
        }
    } else if (bnfMode) {
        if (filename.empty()) {
            cerr << "Please provide a filename using the -bnf flag." << endl;
            return 1;
        }

        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return 1;
        }

        vector<string> bnfClauses;
        string line;
        while (getline(file, line)) {
            bnfClauses.push_back(line);
        }
        file.close();

        CNFConverter converter1;
        vector<string> cnfClauses1 = converter1.convertBnf(bnfClauses);

        if (verboseMode) {
            for (const string &clause: cnfClauses1) {
                cout << clause << endl;
            }
        }
        vector<vector<string>> inputForDPLL = convertToDPLLInput(cnfClauses1);


        map<string, bool> initialAssignments;

        map<string, bool> assignments = dpll(inputForDPLL, initialAssignments);
        for (const auto &assignment: assignments) {
            cout << assignment.first << " = " << (assignment.second ? "true" : "false") << endl;
        }
        cout << "other elements are arbitrary,if exists" << endl;

    }
    return 0;
}






