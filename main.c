#include <stdio.h>
#include <stdlib.h>
#include "maze.h"

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

int main(void) {
    int running = 1;
    while (running) {
        print_menu();
        switch (read_choice()) {
            case 1:
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
