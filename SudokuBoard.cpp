//
// Created by yitong on 2023/10/25.
//

#include "SudokuBoard.h"
#include <stdexcept> // Include for std::out_of_range

SudokuBoard::SudokuBoard() {
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            board[i][j] = 0;  // Initialize with empty cells
}

void SudokuBoard::setCell(int row, int col, int val) {
    board[row][col] = val;
}

int SudokuBoard::getCell(int row, int col) const {
    if (row > 0 && row <= 9 && col > 0 && col <= 9) {
        return board[row - 1][col - 1];
    } else {
        throw std::out_of_range("Row and Column indices should be between 1 and 9 (inclusive).");
    }
}

void SudokuBoard::printBoard() const {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            cout << board[i][j] << " ";
        }
        cout << "\n";
    }
}
