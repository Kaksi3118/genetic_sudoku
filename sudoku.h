#ifndef SUDOKU_H
#define SUDOKU_H
#define MAX_N 16

void generateSudoku(int puzzle[MAX_N][MAX_N],
                    int solution[MAX_N][MAX_N],
                    int hints,
                    int n);

#endif