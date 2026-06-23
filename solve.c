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

static const int turn_row[4] = {-1, 0, 1, 0};
static const int turn_col[4] = {0, 1, 0, -1};

static int manhattan(int row, int col, int goal_row, int goal_col) {
    int delta_row = row - goal_row;
    int delta_col = col - goal_col;
    if (delta_row < 0) {
        delta_row = -delta_row;
    }
    if (delta_col < 0) {
        delta_col = -delta_col;
    }
    return delta_row + delta_col;
}

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

static SolveStats run_astar(const Maze *maze, int animate) {
    char mark[MAZE_MAX_H][MAZE_MAX_W];
    char open_set[MAZE_MAX_H][MAZE_MAX_W];
    char closed_set[MAZE_MAX_H][MAZE_MAX_W];
    int gscore[MAZE_MAX_H][MAZE_MAX_W];
    int parent_row[MAZE_MAX_H][MAZE_MAX_W];
    int parent_col[MAZE_MAX_H][MAZE_MAX_W];
    int goal_row = maze->height - 1;
    int goal_col = maze->width - 1;
    int row, col, i;
    SolveStats stats = {0, 0, 0};
    memset(mark, 0, sizeof mark);
    memset(open_set, 0, sizeof open_set);
    memset(closed_set, 0, sizeof closed_set);
    for (row = 0; row < maze->height; row++) {
        for (col = 0; col < maze->width; col++) {
            gscore[row][col] = CELL_COUNT * 4;
        }
    }
    gscore[0][0] = 0;
    open_set[0][0] = 1;
    mark[0][0] = 'F';
    parent_row[0][0] = -1;
    parent_col[0][0] = -1;
    while (1) {
        int best_row = -1;
        int best_col = -1;
        int best_score = CELL_COUNT * 8;
        for (row = 0; row < maze->height; row++) {
            for (col = 0; col < maze->width; col++) {
                if (open_set[row][col]) {
                    int score = gscore[row][col] +
                                manhattan(row, col, goal_row, goal_col);
                    if (score < best_score) {
                        best_score = score;
                        best_row = row;
                        best_col = col;
                    }
                }
            }
        }
        if (best_row == -1) {
            break;
        }
        open_set[best_row][best_col] = 0;
        closed_set[best_row][best_col] = 1;
        mark[best_row][best_col] = 'V';
        stats.explored++;
        if (animate) {
            render(maze, mark, best_row, best_col);
        }
        if (best_row == goal_row && best_col == goal_col) {
            stats.found = 1;
            break;
        }
        for (i = 0; i < 4; i++) {
            int next_row = best_row + move_row[i];
            int next_col = best_col + move_col[i];
            if (next_row < 0 || next_row >= maze->height ||
                next_col < 0 || next_col >= maze->width) {
                continue;
            }
            if (maze->cells[next_row][next_col] != EMPTY ||
                closed_set[next_row][next_col]) {
                continue;
            }
            if (gscore[best_row][best_col] + 1 < gscore[next_row][next_col]) {
                gscore[next_row][next_col] = gscore[best_row][best_col] + 1;
                parent_row[next_row][next_col] = best_row;
                parent_col[next_row][next_col] = best_col;
                if (!open_set[next_row][next_col]) {
                    open_set[next_row][next_col] = 1;
                    if (mark[next_row][next_col] == 0) {
                        mark[next_row][next_col] = 'F';
                    }
                }
            }
        }
    }
    if (stats.found) {
        trace_path(mark, parent_row, parent_col, goal_row, goal_col, &stats);
        if (animate) {
            render(maze, mark, goal_row, goal_col);
        }
    }
    return stats;
}

static SolveStats run_wall_follower(const Maze *maze, int animate) {
    char mark[MAZE_MAX_H][MAZE_MAX_W];
    int row = 0;
    int col = 0;
    int facing = 2;
    int goal_row = maze->height - 1;
    int goal_col = maze->width - 1;
    int budget = maze->width * maze->height * 8;
    SolveStats stats = {0, 0, 0};
    memset(mark, 0, sizeof mark);
    while (budget > 0) {
        int order[4];
        int moved = 0;
        int k;
        budget--;
        mark[row][col] = 'V';
        stats.explored++;
        if (animate) {
            render(maze, mark, row, col);
        }
        if (row == goal_row && col == goal_col) {
            stats.found = 1;
            break;
        }
        order[0] = (facing + 1) % 4;
        order[1] = facing;
        order[2] = (facing + 3) % 4;
        order[3] = (facing + 2) % 4;
        for (k = 0; k < 4; k++) {
            int direction = order[k];
            int next_row = row + turn_row[direction];
            int next_col = col + turn_col[direction];
            if (next_row >= 0 && next_row < maze->height &&
                next_col >= 0 && next_col < maze->width &&
                maze->cells[next_row][next_col] == EMPTY) {
                facing = direction;
                row = next_row;
                col = next_col;
                moved = 1;
                break;
            }
        }
        if (!moved) {
            break;
        }
    }
    stats.path_length = stats.explored;
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

void solve_astar(const Maze *maze) {
    report("A* (shortest path)", run_astar(maze, 1));
}

void solve_wall_follower(const Maze *maze) {
    report("Right-hand wall follower", run_wall_follower(maze, 1));
}

void solve_compare(const Maze *maze) {
    SolveStats dfs = run_dfs(maze, 0);
    SolveStats bfs = run_bfs(maze, 0);
    SolveStats astar = run_astar(maze, 0);
    SolveStats wall = run_wall_follower(maze, 0);
    clear_screen();
    draw_maze(maze);
    printf("\n%-28s %-10s %-10s\n", "Algorithm", "Explored", "Path");
    printf("%-28s %-10d %-10d\n", "DFS (depth-first)", dfs.explored, dfs.path_length);
    printf("%-28s %-10d %-10d\n", "BFS (shortest path)", bfs.explored, bfs.path_length);
    printf("%-28s %-10d %-10d\n", "A* (shortest path)", astar.explored, astar.path_length);
    printf("%-28s %-10d %-10d\n", "Right-hand wall follower",
           wall.explored, wall.path_length);
}
