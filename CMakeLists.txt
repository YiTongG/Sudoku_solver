cmake_minimum_required(VERSION 2.8)
project(AIlab2)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(AIlab2 main.cpp SudokuBoard.cpp DPLL.cpp
        SudokuBoard.cpp
        CNFConverter.cpp)
