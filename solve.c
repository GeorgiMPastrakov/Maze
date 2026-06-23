#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "maze.h"

#define CELL_COUNT (MAZE_MAX_H * MAZE_MAX_W)
#define STEP_DELAY 35000

#define COLOR_RESET "\033[0m"
#define COLOR_DIM "\033[90m"
#define COLOR_CYAN "\033[36m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_RED "\033[1;31m"

typedef struct {
    int found;
    int explored;
    int path_length;
} SolveStats;

static const int move_row[4] = {-1, 1, 0, 0};
static const int move_col[4] = {0, 0, -1, 1};

static void render(const Maze *maze, char mark[MAZE_MAX_H][MAZE_MAX_W],
                   int current_row, int current_col) {
    int row, col;
    clear_screen();
    for (row = 0; row < maze->height; row++) {
        for (col = 0; col < maze->width; col++) {
            if (row == current_row && col == current_col) {
                printf(COLOR_YELLOW "@" COLOR_RESET);
            } else if (row == 0 && col == 0) {
                printf(COLOR_GREEN "S" COLOR_RESET);
            } else if (row == maze->height - 1 && col == maze->width - 1) {
                printf(COLOR_RED "E" COLOR_RESET);
            } else if (mark[row][col] == 'P') {
                printf(COLOR_GREEN "o" COLOR_RESET);
            } else if (mark[row][col] == 'F') {
                printf(COLOR_CYAN "+" COLOR_RESET);
            } else if (mark[row][col] == 'V') {
                printf(COLOR_DIM "." COLOR_RESET);
            } else if (maze->cells[row][col] == WALL) {
                putchar('#');
            } else {
                putchar(' ');
            }
        }
        putchar('\n');
    }
    usleep(STEP_DELAY);
}

static void trace_path(char mark[MAZE_MAX_H][MAZE_MAX_W],
                       int parent_row[MAZE_MAX_H][MAZE_MAX_W],
                       int parent_col[MAZE_MAX_H][MAZE_MAX_W],
                       int goal_row, int goal_col, SolveStats *stats) {
    int row = goal_row;
    int col = goal_col;
    while (row != -1) {
        int previous_row = parent_row[row][col];
        int previous_col = parent_col[row][col];
        mark[row][col] = 'P';
        stats->path_length++;
        row = previous_row;
        col = previous_col;
    }
}

static SolveStats run_dfs(const Maze *maze, int animate) {
    char mark[MAZE_MAX_H][MAZE_MAX_W];
    char discovered[MAZE_MAX_H][MAZE_MAX_W];
    char visited[MAZE_MAX_H][MAZE_MAX_W];
    int parent_row[MAZE_MAX_H][MAZE_MAX_W];
    int parent_col[MAZE_MAX_H][MAZE_MAX_W];
    int stack_row[CELL_COUNT];
    int stack_col[CELL_COUNT];
    int top = 0;
    int i;
    SolveStats stats = {0, 0, 0};
    memset(mark, 0, sizeof mark);
    memset(discovered, 0, sizeof discovered);
    memset(visited, 0, sizeof visited);
    stack_row[top] = 0;
    stack_col[top] = 0;
    top++;
    discovered[0][0] = 1;
    parent_row[0][0] = -1;
    parent_col[0][0] = -1;
    while (top > 0) {
        int row = stack_row[--top];
        int col = stack_col[top];
        if (visited[row][col]) {
            continue;
        }
        visited[row][col] = 1;
        mark[row][col] = 'V';
        stats.explored++;
        if (animate) {
            render(maze, mark, row, col);
        }
        if (row == maze->height - 1 && col == maze->width - 1) {
            stats.found = 1;
            break;
        }
        for (i = 0; i < 4; i++) {
            int next_row = row + move_row[i];
            int next_col = col + move_col[i];
            if (next_row >= 0 && next_row < maze->height &&
                next_col >= 0 && next_col < maze->width &&
                maze->cells[next_row][next_col] == EMPTY &&
                !discovered[next_row][next_col]) {
                discovered[next_row][next_col] = 1;
                parent_row[next_row][next_col] = row;
                parent_col[next_row][next_col] = col;
                stack_row[top] = next_row;
                stack_col[top] = next_col;
                top++;
            }
        }
    }
    if (stats.found) {
        trace_path(mark, parent_row, parent_col,
                   maze->height - 1, maze->width - 1, &stats);
        if (animate) {
            render(maze, mark, maze->height - 1, maze->width - 1);
        }
    }
    return stats;
}

static SolveStats run_bfs(const Maze *maze, int animate) {
    char mark[MAZE_MAX_H][MAZE_MAX_W];
    char visited[MAZE_MAX_H][MAZE_MAX_W];
    int parent_row[MAZE_MAX_H][MAZE_MAX_W];
    int parent_col[MAZE_MAX_H][MAZE_MAX_W];
    int queue_row[CELL_COUNT];
    int queue_col[CELL_COUNT];
    int head = 0;
    int tail = 0;
    int i;
    SolveStats stats = {0, 0, 0};
    memset(mark, 0, sizeof mark);
    memset(visited, 0, sizeof visited);
    visited[0][0] = 1;
    parent_row[0][0] = -1;
    parent_col[0][0] = -1;
    queue_row[tail] = 0;
    queue_col[tail] = 0;
    tail++;
    while (head < tail) {
        int row = queue_row[head];
        int col = queue_col[head];
        head++;
        mark[row][col] = 'V';
        stats.explored++;
        if (animate) {
            render(maze, mark, row, col);
        }
        if (row == maze->height - 1 && col == maze->width - 1) {
            stats.found = 1;
            break;
        }
        for (i = 0; i < 4; i++) {
            int next_row = row + move_row[i];
            int next_col = col + move_col[i];
            if (next_row >= 0 && next_row < maze->height &&
                next_col >= 0 && next_col < maze->width &&
                maze->cells[next_row][next_col] == EMPTY &&
                !visited[next_row][next_col]) {
                visited[next_row][next_col] = 1;
                parent_row[next_row][next_col] = row;
                parent_col[next_row][next_col] = col;
                queue_row[tail] = next_row;
                queue_col[tail] = next_col;
                tail++;
            }
        }
    }
    if (stats.found) {
        trace_path(mark, parent_row, parent_col,
                   maze->height - 1, maze->width - 1, &stats);
        if (animate) {
            render(maze, mark, maze->height - 1, maze->width - 1);
        }
    }
    return stats;
}

static void report(const char *name, SolveStats stats) {
    if (stats.found) {
        printf("%s: reached the goal, explored %d cells, path length %d\n",
               name, stats.explored, stats.path_length);
    } else {
        printf("%s: no path found\n", name);
    }
}

void solve_dfs(const Maze *maze) {
    report("DFS (depth-first)", run_dfs(maze, 1));
}

void solve_bfs(const Maze *maze) {
    report("BFS (shortest path)", run_bfs(maze, 1));
}
