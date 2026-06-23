#include <stdio.h>
#include "maze.h"

void player_traverse(const Maze *maze) {
    int row = 0;
    int col = 0;
    char line[64];
    while (1) {
        clear_screen();
        draw_maze_marked(maze, row, col, '@');
        printf("Move (w/a/s/d, q to quit): ");
        if (fgets(line, sizeof line, stdin) == NULL) {
            return;
        }
        int next_row = row;
        int next_col = col;
        switch (line[0]) {
            case 'w':
                next_row--;
                break;
            case 's':
                next_row++;
                break;
            case 'a':
                next_col--;
                break;
            case 'd':
                next_col++;
                break;
            case 'q':
                return;
            default:
                continue;
        }
        if (next_row >= 0 && next_row < maze->height &&
            next_col >= 0 && next_col < maze->width &&
            maze->cells[next_row][next_col] == EMPTY) {
            row = next_row;
            col = next_col;
        }
        if (row == maze->height - 1 && col == maze->width - 1) {
            clear_screen();
            draw_maze_marked(maze, row, col, '@');
            printf("You reached the goal!\n");
            return;
        }
    }
}
