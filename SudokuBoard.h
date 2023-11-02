//
// Created by yitong on 2023/10/25.
//

#ifndef AILAB2_SUDOKUBOARD_H
#define AILAB2_SUDOKUBOARD_H


#include <iostream>
#include <vector>

using namespace std;

class SudokuBoard {
private:
    //int board[9][9];

public:
    int board[9][9];
    SudokuBoard();

    void setCell(int row, int col, int val);
    int getCell(int row, int col) const;
    void printBoard() const;
};


#endif //AILAB2_SUDOKUBOARD_H
