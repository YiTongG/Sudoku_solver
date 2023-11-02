cmake_minimum_required(VERSION 2.8)
project(AIlab2)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()
add_executable(AIlab2 main.cpp SudokuBoard.cpp DPLL.cpp
        SudokuBoard.cpp
        CNFConverter.cpp)
