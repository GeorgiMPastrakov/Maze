#include <time.h>
#include "maze.h"

typedef struct {
    unsigned state;
} Rng;

static unsigned rng_next(Rng *rng) {
    unsigned x = rng->state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng->state = x;
    return x;
}

static int rng_range(Rng *rng, int bound) {
    return (int)(rng_next(rng) % (unsigned)bound);
}

static void shuffle_directions(int *order, Rng *rng) {
    int i;
    for (i = 0; i < 4; i++) {
        order[i] = i;
    }
    for (i = 3; i > 0; i--) {
        int j = rng_range(rng, i + 1);
        int tmp = order[i];
        order[i] = order[j];
        order[j] = tmp;
    }
}

static void carve(Maze *maze, int row, int col, Rng *rng) {
    static const int step_row[4] = {-2, 2, 0, 0};
    static const int step_col[4] = {0, 0, -2, 2};
    int order[4];
    int i;
    maze->cells[row][col] = EMPTY;
    shuffle_directions(order, rng);
    for (i = 0; i < 4; i++) {
        int dir = order[i];
        int next_row = row + step_row[dir];
        int next_col = col + step_col[dir];
        if (next_row >= 0 && next_row < maze->height &&
            next_col >= 0 && next_col < maze->width &&
            maze->cells[next_row][next_col] == WALL) {
            maze->cells[row + step_row[dir] / 2][col + step_col[dir] / 2] = EMPTY;
            carve(maze, next_row, next_col, rng);
        }
    }
}

static int clamp(int value, int low, int high) {
    if (value < low) {
        return low;
    }
    if (value > high) {
        return high;
    }
    return value;
}

static int snap_to_room(int value, int limit) {
    int snapped = clamp(value, 0, limit - 1);
    if (snapped % 2 != 0) {
        snapped = (snapped + 1 < limit) ? snapped + 1 : snapped - 1;
    }
    return snapped;
}

static void set_exit(Maze *maze, int exit_row, int exit_col) {
    int row = snap_to_room(exit_row, maze->height);
    int col = snap_to_room(exit_col, maze->width);
    if (row == maze->start_row && col == maze->start_col) {
        row = snap_to_room(maze->height - 1, maze->height);
        col = snap_to_room(maze->width - 1, maze->width);
    }
    maze->goal_row = row;
    maze->goal_col = col;
    maze->cells[row][col] = EMPTY;
}

static void add_loops(Maze *maze, int count, Rng *rng) {
    int candidate_row[MAZE_MAX_H * MAZE_MAX_W];
    int candidate_col[MAZE_MAX_H * MAZE_MAX_W];
    int total = 0;
    int row, col, i;
    for (row = 0; row < maze->height; row++) {
        for (col = 0; col < maze->width; col++) {
            if (maze->cells[row][col] != WALL) {
                continue;
            }
            if (row % 2 == 0 && col % 2 == 1 && col + 1 < maze->width &&
                maze->cells[row][col - 1] == EMPTY &&
                maze->cells[row][col + 1] == EMPTY) {
                candidate_row[total] = row;
                candidate_col[total] = col;
                total++;
            } else if (row % 2 == 1 && col % 2 == 0 && row + 1 < maze->height &&
                       maze->cells[row - 1][col] == EMPTY &&
                       maze->cells[row + 1][col] == EMPTY) {
                candidate_row[total] = row;
                candidate_col[total] = col;
                total++;
            }
        }
    }
    for (i = total - 1; i > 0; i--) {
        int j = rng_range(rng, i + 1);
        int temp_row = candidate_row[i];
        int temp_col = candidate_col[i];
        candidate_row[i] = candidate_row[j];
        candidate_col[i] = candidate_col[j];
        candidate_row[j] = temp_row;
        candidate_col[j] = temp_col;
    }
    if (count > total) {
        count = total;
    }
    for (i = 0; i < count; i++) {
        maze->cells[candidate_row[i]][candidate_col[i]] = EMPTY;
    }
}

void generate_maze(Maze *maze, int width, int height,
                   int exit_row, int exit_col, int extra_paths, unsigned seed) {
    Rng rng;
    rng.state = seed != 0u ? seed : (unsigned)time(NULL);
    maze->width = clamp(width, MAZE_MIN, MAZE_MAX_W);
    maze->height = clamp(height, MAZE_MIN, MAZE_MAX_H);
    maze->start_row = 0;
    maze->start_col = 0;
    fill_walls(maze);
    carve(maze, 0, 0, &rng);
    maze->cells[0][0] = EMPTY;
    set_exit(maze, exit_row, exit_col);
    if (extra_paths > 0) {
        add_loops(maze, extra_paths, &rng);
    }
}
