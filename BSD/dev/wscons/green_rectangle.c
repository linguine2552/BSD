#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#define ANSI_COLOR_GREEN   "\x1b[42m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int main() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    printf("Terminal size: %d rows, %d columns\n", w.ws_row, w.ws_col);

    int rect_height = w.ws_row / 2;
    int rect_width = w.ws_col / 2;

    // Clear the screen
    printf("\x1b[2J");

    // Move cursor to top-left corner
    printf("\x1b[H");

    // Draw the green rectangle
    for (int y = 0; y < rect_height; y++) {
        for (int x = 0; x < rect_width; x++) {
            printf(ANSI_COLOR_GREEN " " ANSI_COLOR_RESET);
        }
        printf("\n");
    }

    getchar();

    // Clear the screen again before exiting
    printf("\x1b[2J");
    printf("\x1b[H");

    return 0;
}
