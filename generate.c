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

void generate_maze(Maze *maze, int width, int height, unsigned seed) {
    Rng rng;
    rng.state = seed != 0u ? seed : (unsigned)time(NULL);
    maze->width = width;
    maze->height = height;
    fill_walls(maze);
    carve(maze, 0, 0, &rng);
    maze->cells[height - 1][width - 1] = EMPTY;
}
