#ifndef MAZE_H
#define MAZE_H

#define MAZE_MIN 3
#define MAZE_MAX_W 40
#define MAZE_MAX_H 30

#define WALL '#'
#define EMPTY ' '

typedef struct {
    int width;
    int height;
    int start_row;
    int start_col;
    int goal_row;
    int goal_col;
    char cells[MAZE_MAX_H][MAZE_MAX_W];
} Maze;

void fill_walls(Maze *maze);
void draw_maze(const Maze *maze);
void draw_maze_marked(const Maze *maze, int row, int col, char glyph);
void clear_screen(void);

void generate_maze(Maze *maze, int width, int height,
                   int exit_row, int exit_col, int extra_paths, unsigned seed);

int write_maze(const Maze *maze, const char *filename);
int read_maze(Maze *maze, const char *filename);

void player_traverse(const Maze *maze);

void solve_dfs(const Maze *maze);
void solve_bfs(const Maze *maze);
void solve_astar(const Maze *maze);
void solve_wall_follower(const Maze *maze);
void solve_compare(const Maze *maze);

#endif
