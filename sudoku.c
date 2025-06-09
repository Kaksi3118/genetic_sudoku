#include "sudoku.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

static int unUsedInBox(int grid[MAX_N][MAX_N], int rowStart, int colStart, int num, int sr) {
    for (int i = 0; i < sr; i++)
        for (int j = 0; j < sr; j++)
            if (grid[rowStart+i][colStart+j] == num)
                return 0;
    return 1;
}
static void fillBox(int grid[MAX_N][MAX_N], int row, int col, int sr) {
    int num;
    int max_val = sr * sr;
    for (int i = 0; i < sr; i++) {
        for (int j = 0; j < sr; j++) {
            do {
                num = (rand() % max_val) + 1;
            } while (!unUsedInBox(grid, row, col, num, sr));
            grid[row + i][col + j] = num;
        }
    }
}
static int usedInRow(int grid[MAX_N][MAX_N], int row, int num, int n) {
    for (int c = 0; c < n; c++)
        if (grid[row][c] == num)
            return 1;
    return 0;
}
static int usedInCol(int grid[MAX_N][MAX_N], int col, int num, int n) {
    for (int r = 0; r < n; r++)
        if (grid[r][col] == num)
            return 1;
    return 0;
}
static int checkIfSafe(int grid[MAX_N][MAX_N], int row, int col, int num, int n) {
    int sr = (int)sqrt(n);
    return !usedInRow(grid, row, num, n)
        && !usedInCol(grid, col, num, n)
        &&  unUsedInBox(grid, row - row % sr, col - col % sr, num, sr);
}
static void fillDiagonal(int grid[MAX_N][MAX_N], int n, int sr) {
    for (int i = 0; i < n; i += sr)
        fillBox(grid, i, i, sr);
}
static int fillRemaining(int grid[MAX_N][MAX_N], int row, int col, int n, int sr) {
    if (row == n) return 1;
    if (col == n) return fillRemaining(grid, row+1, 0, n, sr);
    if (grid[row][col] != 0) return fillRemaining(grid, row, col+1, n, sr);

    for (int num = 1; num <= n; num++) {
        if (checkIfSafe(grid, row, col, num, n)) {
            grid[row][col] = num;
            if (fillRemaining(grid, row, col+1, n, sr)) return 1;
            grid[row][col] = 0;
        }
    }
    return 0;
}
static void removeKDigits(int grid[MAX_N][MAX_N], int k, int n) {
    while (k > 0) {
        int cell = rand() % (n*n);
        int i = cell / n, j = cell % n;
        if (grid[i][j] != 0) {
            grid[i][j] = 0;
            k--;
        }
    }
}
void generateSudoku(int puzzle[MAX_N][MAX_N],
                    int solution[MAX_N][MAX_N],
                    int hints,
                    int n)
{
    int sr = (int)sqrt(n);
    srand((unsigned)time(NULL));
    while (1) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                solution[i][j] = 0;

        fillDiagonal(solution, n, sr);
        if (fillRemaining(solution, 0, 0, n, sr))
            break;
    }
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            puzzle[i][j] = solution[i][j];

    removeKDigits(puzzle, n*n - hints, n);
}