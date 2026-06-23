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

static void carve_border_path(Maze *maze, Rng *rng) {
    int i;
    int w = maze->width;
    int h = maze->height;
    if (rng_range(rng, 2) == 0) {
        for (i = 0; i < w; i++) {
            maze->cells[0][i] = EMPTY;
        }
        for (i = 0; i < h; i++) {
            maze->cells[i][w - 1] = EMPTY;
        }
    } else {
        for (i = 0; i < h; i++) {
            maze->cells[i][0] = EMPTY;
        }
        for (i = 0; i < w; i++) {
            maze->cells[h - 1][i] = EMPTY;
        }
    }
}

void generate_maze(Maze *maze, int width, int height, unsigned seed) {
    Rng rng;
    rng.state = seed != 0u ? seed : (unsigned)time(NULL);
    maze->width = width;
    maze->height = height;
    fill_walls(maze);
    carve_border_path(maze, &rng);
}
