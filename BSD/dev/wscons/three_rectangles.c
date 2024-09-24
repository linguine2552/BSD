#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLACK   "\x1b[40m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void draw_rectangle(int start_row, int start_col, int height, int width) {
    for (int y = 0; y < height; y++) {
        printf("\x1b[%d;%dH", start_row + y, start_col);
        for (int x = 0; x < width; x++) {
            if (y == 0 || y == height - 1 || x == 0 || x == width - 1) {
                printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET, "█");
            } else {
                printf(ANSI_COLOR_BLACK " " ANSI_COLOR_RESET);
            }
        }
    }
}

int main() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // Clear the screen
    printf("\x1b[2J");

    // Move cursor to top-left corner
    printf("\x1b[H");

    // Calculate dimensions
    int top_rect_height = w.ws_row / 6;
    int left_rect_width = (w.ws_col * 3) / 4 - 1; // -1 for border
    int right_rect_width = w.ws_col - left_rect_width - 2; // -2 for borders
    int bottom_rect_height = w.ws_row - top_rect_height - 2; // -2 for borders

    // Draw rectangles
    draw_rectangle(1, 1, top_rect_height, w.ws_col);
    draw_rectangle(top_rect_height + 2, 1, bottom_rect_height, left_rect_width);
    draw_rectangle(top_rect_height + 2, left_rect_width + 2, bottom_rect_height, right_rect_width);

    // Move cursor to bottom of the screen
    printf("\x1b[%d;1H", w.ws_row);
    printf("Three black rectangles with green outlines drawn. Press Enter to exit...");
    getchar();

    // Clear the screen again before exiting
    printf("\x1b[2J");
    printf("\x1b[H");

    return 0;
}
