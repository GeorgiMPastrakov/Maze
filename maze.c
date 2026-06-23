#include <stdio.h>
#include "maze.h"

void clear_screen(void) {
    printf("\033[2J\033[H");
}

void fill_walls(Maze *maze) {
    int row, col;
    for (row = 0; row < MAZE_MAX_H; row++) {
        for (col = 0; col < MAZE_MAX_W; col++) {
            maze->cells[row][col] = WALL;
        }
    }
}

void draw_maze(const Maze *maze) {
    int row, col;
    for (row = 0; row < maze->height; row++) {
        for (col = 0; col < maze->width; col++) {
            putchar(maze->cells[row][col]);
        }
        putchar('\n');
    }
}

void draw_maze_marked(const Maze *maze, int row, int col, char glyph) {
    int r, c;
    for (r = 0; r < maze->height; r++) {
        for (c = 0; c < maze->width; c++) {
            if (r == row && c == col) {
                putchar(glyph);
            } else if (r == maze->goal_row && c == maze->goal_col) {
                putchar('E');
            } else {
                putchar(maze->cells[r][c]);
            }
        }
        putchar('\n');
    }
}
