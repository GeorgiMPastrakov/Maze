#include <stdio.h>
#include "maze.h"

int write_maze(const Maze *maze, const char *filename) {
    FILE *file;
    int row, col;
    if (maze == NULL || filename == NULL) {
        return 0;
    }
    file = fopen(filename, "w");
    if (file == NULL) {
        return 0;
    }
    if (fprintf(file, "%d %d\n", maze->width, maze->height) < 0) {
        fclose(file);
        return 0;
    }
    for (row = 0; row < maze->height; row++) {
        for (col = 0; col < maze->width; col++) {
            if (fputc(maze->cells[row][col], file) == EOF) {
                fclose(file);
                return 0;
            }
        }
        if (fputc('\n', file) == EOF) {
            fclose(file);
            return 0;
        }
    }
    fclose(file);
    return 1;
}
