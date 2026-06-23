#include <stdio.h>
#include "maze.h"

void clear_screen(void) {
    printf("\033[2J\033[H");
}

void fill_walls(Maze *maze) {
    int row, col;
    for (row = 0; row < maze->height; row++) {
        for (col = 0; col < maze->width; col++) {
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
