#include <stdio.h>
#include <stdlib.h>
#include "maze.h"

static void read_line(char *buffer, int size) {
    if (fgets(buffer, size, stdin) == NULL) {
        buffer[0] = '\0';
    }
}

static int prompt_int(const char *label) {
    char line[64];
    printf("%s", label);
    read_line(line, sizeof line);
    return atoi(line);
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
    int seed = prompt_int("Seed (0 = random): ");
    generate_maze(maze, width, height, (unsigned)seed);
    *has_maze = 1;
    clear_screen();
    draw_maze(maze);
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
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
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
