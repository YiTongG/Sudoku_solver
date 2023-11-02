

# AIlab2 Sudoku Solver

The `AIlab2` Sudoku Solver is a command-line utility designed to solve Sudoku puzzles using the DPLL algorithm.

## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Verbose Mode](#verbose-mode)
- [Output Files](#output-files)

## Installation

1. Change to code directory: `cd /path/to/AIlab2`.
2. Build the program:
   ```bash
   mkdir build && cd build && cmake .. && make
   ```

## Usage

Once the project is compiled, you can run the Sudoku Solver using the following command:

```sh
./AIlab2 [options] puzzle_input
```

Replace `puzzle_input` with the initial Sudoku clues in the format `rc=v`, where `r` is the row, `c` is the column, and `v` is the value to be placed in that cell. Ensure that all provided clues are valid with values between 1 and 9.

For example:

```sh
./AIlab2 11=4 12=5 23=2 25=7 27=6 28=3 38=2 39=8 44=9 45=5 52=8 53=6 57=2 62=2 64=6 67=7 68=5 77=4 78=7 79=6 82=7 85=4 86=5 93=8 96=9
```

## Verbose Mode

The `-v` option enables verbose mode, which provides additional details during execution:

```sh
./AIlab2 -v puzzle_input
```

In verbose mode, the following actions will occur:

1. The CNF (Conjunctive Normal Form) clauses of the Sudoku puzzle will be printed to a file named `cnfForSudoku1.txt` in the current directory.

2. The results of the DPLL algorithm will be output to a file named `dp_output.txt` in the same directory.

## Output Files

- **cnfForSudoku1.txt**: Contains the CNF representation of the Sudoku puzzle.

- **dp_output.txt**: Contains the results from the DPLL algorithm, if verbose mode is enabled.

---

Ensure that you replace `puzzle_input` with the actual inputs for your Sudoku puzzle when running the solver.

Certainly, here's a concise version of the usage documentation:

---

# Extra Credit


**Run:**
- Execute the program with BNF input file:
  ```bash
  ./AIlab2 -bnf input.txt
  ```
- For verbose output (BNF to CNF conversion):
  ```bash
  ./AIlab2 -v -bnf input.txt
  ```

Verbose mode (`-v`) provides the CNF clauses in the output.

**Note:** Replace `input.txt` with the path to your actual BNF file.

---
