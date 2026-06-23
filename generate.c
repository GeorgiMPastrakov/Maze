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

static void open_exit(Maze *maze) {
    int last_row = maze->height - 1;
    int last_col = maze->width - 1;
    int even_row = (last_row % 2 == 0) ? last_row : last_row - 1;
    int even_col = (last_col % 2 == 0) ? last_col : last_col - 1;
    int row, col;
    for (col = even_col; col <= last_col; col++) {
        maze->cells[even_row][col] = EMPTY;
    }
    for (row = even_row; row <= last_row; row++) {
        maze->cells[row][last_col] = EMPTY;
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

void generate_maze(Maze *maze, int width, int height, unsigned seed) {
    Rng rng;
    rng.state = seed != 0u ? seed : (unsigned)time(NULL);
    maze->width = clamp(width, MAZE_MIN, MAZE_MAX_W);
    maze->height = clamp(height, MAZE_MIN, MAZE_MAX_H);
    fill_walls(maze);
    carve(maze, 0, 0, &rng);
    maze->cells[0][0] = EMPTY;
    open_exit(maze);
}
