#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "maze.h"

#define CELL_COUNT (MAZE_MAX_H * MAZE_MAX_W)

static const int move_row[4] = {-1, 1, 0, 0};
static const int move_col[4] = {0, 0, -1, 1};

static void animate(const Maze *maze, const char visited[MAZE_MAX_H][MAZE_MAX_W],
                    int row, int col) {
    Maze view = *maze;
    int r, c;
    for (r = 0; r < maze->height; r++) {
        for (c = 0; c < maze->width; c++) {
            if (visited[r][c]) {
                view.cells[r][c] = '*';
            }
        }
    }
    view.cells[row][col] = '@';
    clear_screen();
    draw_maze(&view);
    usleep(40000);
}

void solve_dfs(const Maze *maze) {
    char visited[MAZE_MAX_H][MAZE_MAX_W];
    int stack_row[CELL_COUNT];
    int stack_col[CELL_COUNT];
    int top = 0;
    int i;
    memset(visited, 0, sizeof visited);
    stack_row[top] = 0;
    stack_col[top] = 0;
    top++;
    while (top > 0) {
        int row = stack_row[--top];
        int col = stack_col[top];
        if (visited[row][col]) {
            continue;
        }
        visited[row][col] = 1;
        animate(maze, visited, row, col);
        if (row == maze->height - 1 && col == maze->width - 1) {
            printf("Reached the goal\n");
            return;
        }
        for (i = 0; i < 4; i++) {
            int next_row = row + move_row[i];
            int next_col = col + move_col[i];
            if (next_row >= 0 && next_row < maze->height &&
                next_col >= 0 && next_col < maze->width &&
                maze->cells[next_row][next_col] == EMPTY &&
                !visited[next_row][next_col]) {
                stack_row[top] = next_row;
                stack_col[top] = next_col;
                top++;
            }
        }
    }
    printf("No path found\n");
}
