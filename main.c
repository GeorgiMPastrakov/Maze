#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "maze.h"

static void read_line(char *buffer, int size) {
    if (fgets(buffer, size, stdin) == NULL) {
        buffer[0] = '\0';
    }
}

static void prompt_string(const char *label, char *buffer, int size) {
    int len;
    printf("%s", label);
    read_line(buffer, size);
    len = (int)strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
        buffer[--len] = '\0';
    }
}

static int prompt_int(const char *label) {
    char line[64];
    printf("%s", label);
    read_line(line, sizeof line);
    return atoi(line);
}

static void pause_enter(void) {
    char line[64];
    printf("Press Enter to continue...");
    read_line(line, sizeof line);
}

static void print_menu(void) {
    printf("\n=== Maze ===\n");
    printf("1. Generate maze\n");
    printf("2. Read from file\n");
    printf("3. Write to file\n");
    printf("4. Player traversal\n");
    printf("5. Computer traversal\n");
    printf("6. Exit\n");
    printf("Choice: ");
}

static int read_choice(void) {
    char line[64];
    if (fgets(line, sizeof line, stdin) == NULL) {
        return 6;
    }
    return atoi(line);
}

static void handle_generate(Maze *maze, int *has_maze) {
    int width = prompt_int("Width (3-40): ");
    int height = prompt_int("Height (3-30): ");
    int exit_row = prompt_int("Exit row (-1 = bottom-right corner): ");
    int exit_col = prompt_int("Exit col (-1 = bottom-right corner): ");
    int extra_paths = prompt_int("Extra paths (0 = single solution): ");
    int seed = prompt_int("Seed (0 = random): ");
    if (exit_row < 0) {
        exit_row = height - 1;
    }
    if (exit_col < 0) {
        exit_col = width - 1;
    }
    generate_maze(maze, width, height, exit_row, exit_col, extra_paths,
                  (unsigned)seed);
    *has_maze = 1;
    clear_screen();
    draw_maze(maze);
    pause_enter();
}

static void handle_solve(const Maze *maze) {
    int algorithm;
    printf("1. Depth-first search\n");
    printf("2. Breadth-first search (shortest path)\n");
    printf("3. A* search (shortest path)\n");
    printf("4. Right-hand wall follower\n");
    printf("5. Compare all algorithms\n");
    algorithm = prompt_int("Algorithm: ");
    switch (algorithm) {
        case 2:
            solve_bfs(maze);
            break;
        case 3:
            solve_astar(maze);
            break;
        case 4:
            solve_wall_follower(maze);
            break;
        case 5:
            solve_compare(maze);
            break;
        default:
            solve_dfs(maze);
            break;
    }
    pause_enter();
}

static void handle_load(Maze *maze, int *has_maze) {
    char name[256];
    prompt_string("Filename: ", name, sizeof name);
    if (read_maze(maze, name)) {
        *has_maze = 1;
        clear_screen();
        draw_maze(maze);
        pause_enter();
    } else {
        printf("Could not read a valid maze from that file\n");
    }
}

static void handle_save(const Maze *maze, int has_maze) {
    char name[256];
    prompt_string("Filename: ", name, sizeof name);
    if (!has_maze) {
        FILE *file = fopen(name, "w");
        if (file == NULL) {
            printf("Could not open that file\n");
            return;
        }
        fclose(file);
        printf("No maze generated yet; wrote an empty file\n");
        return;
    }
    if (write_maze(maze, name)) {
        printf("Maze saved\n");
    } else {
        printf("Could not write to that file\n");
    }
}

int main(void) {
    Maze maze;
    int has_maze = 0;
    int running = 1;
    while (running) {
        print_menu();
        switch (read_choice()) {
            case 1:
                handle_generate(&maze, &has_maze);
                break;
            case 2:
                handle_load(&maze, &has_maze);
                break;
            case 3:
                handle_save(&maze, has_maze);
                break;
            case 4:
                if (has_maze) {
                    player_traverse(&maze);
                    pause_enter();
                } else {
                    printf("Generate or load a maze first\n");
                }
                break;
            case 5:
                if (has_maze) {
                    handle_solve(&maze);
                } else {
                    printf("Generate or load a maze first\n");
                }
                break;
            case 6:
                running = 0;
                break;
            default:
                printf("Invalid choice\n");
                break;
        }
    }
    return 0;
}
