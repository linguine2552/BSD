#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <dev/wscons/wsconsio.h>
#include <err.h>
#include <errno.h>
#include <string.h>

#define CONSOLE "/dev/ttyC0"
#define RECT_X 10
#define RECT_Y 5
#define RECT_WIDTH 30
#define RECT_HEIGHT 10

void print_error(const char *msg) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
}

int main() {
    int fd = open(CONSOLE, O_RDWR);
    if (fd == -1) {
        print_error("Cannot open console");
        return 1;
    }

    struct wsdisplay_char_cell cell;
    cell.foreground = WSCOL_BLACK;
    cell.background = WSCOL_GREEN;
    cell.char_ = ' '; // Fill with space

    struct wsdisplay_cchar_data cdata;
    cdata.cellsz = sizeof(struct wsdisplay_char_cell);
    cdata.cells = &cell;
    cdata.flags = 0;

    for (int y = RECT_Y; y < RECT_Y + RECT_HEIGHT; y++) {
        for (int x = RECT_X; x < RECT_X + RECT_WIDTH; x++) {
            struct wsdisplay_char_op op;
            op.op = WSDISPLAYIO_SCPOS;
            op.pos = y * 80 + x; // Assuming 80 columns per row
            if (ioctl(fd, WSDISPLAYIO_WCURSOR, &op) == -1) {
                print_error("Failed to set cursor position");
                close(fd);
                return 1;
            }

            if (ioctl(fd, WSDISPLAYIO_PUTCHAR, &cdata) == -1) {
                print_error("Failed to write character");
                close(fd);
                return 1;
            }
        }
    }

    close(fd);
    return 0;
}
