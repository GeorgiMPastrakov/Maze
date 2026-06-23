#include <stdio.h>
#include "maze.h"

int read_maze(Maze *maze, const char *filename) {
    FILE *file;
    int width, height, row, col, ch;
    if (maze == NULL || filename == NULL) {
        return 0;
    }
    file = fopen(filename, "r");
    if (file == NULL) {
        return 0;
    }
    if (fscanf(file, "%d %d", &width, &height) != 2) {
        fclose(file);
        return 0;
    }
    if (width < MAZE_MIN || width > MAZE_MAX_W ||
        height < MAZE_MIN || height > MAZE_MAX_H) {
        fclose(file);
        return 0;
    }
    while ((ch = fgetc(file)) != '\n' && ch != EOF) {
    }
    fill_walls(maze);
    maze->width = width;
    maze->height = height;
    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            ch = fgetc(file);
            if (ch != WALL && ch != EMPTY) {
                fclose(file);
                return 0;
            }
            maze->cells[row][col] = (char)ch;
        }
        ch = fgetc(file);
        if (ch != '\n' && ch != EOF) {
            fclose(file);
            return 0;
        }
    }
    fclose(file);
    return 1;
}

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
