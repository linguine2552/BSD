#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <string.h>
#include <time.h>
#include <dev/wscons/wsconsio.h>
#include <sys/ioctl.h>

#define CONSOLE "/dev/ttyC0"
#define CURSOR_SIZE 32

int main() {
    int fd;
    struct wsdisplay_cursor cursor;
    int mode;
    char shape[(CURSOR_SIZE * CURSOR_SIZE) / 8];

    // Open the console
    fd = open(CONSOLE, O_RDWR);
    if (fd == -1)
        err(1, "Cannot open console");

    // Get current mode
    if (ioctl(fd, WSDISPLAYIO_GMODE, &mode) == -1)
        err(1, "Cannot get mode");

    printf("Current mode: %d\n", mode);

    // Switch to graphics mode if not already
    if (mode != WSDISPLAYIO_MODE_EMUL) {
        mode = WSDISPLAYIO_MODE_EMUL;
        if (ioctl(fd, WSDISPLAYIO_SMODE, &mode) == -1)
            err(1, "Cannot set mode");
    }

    // Initialize cursor structure
    memset(&cursor, 0, sizeof(cursor));
    cursor.which = WSDISPLAY_CURSOR_DOCMAP | WSDISPLAY_CURSOR_DOSHAPE |
                   WSDISPLAY_CURSOR_DOPOS | WSDISPLAY_CURSOR_DOHOT;

    // Set cursor color (white)
    cursor.cmap.red[0] = cursor.cmap.green[0] = cursor.cmap.blue[0] = 0;
    cursor.cmap.red[1] = cursor.cmap.green[1] = cursor.cmap.blue[1] = 255;
    cursor.cmap.index = 0;
    cursor.cmap.count = 2;

    // Set cursor shape (a CURSOR_SIZE x CURSOR_SIZE block)
    memset(shape, 0xff, sizeof(shape));
    cursor.image = shape;
    cursor.mask = shape;
    cursor.size.x = CURSOR_SIZE;
    cursor.size.y = CURSOR_SIZE;

    // Set initial position
    cursor.pos.x = 100;
    cursor.pos.y = 100;

    // Set hotspot
    cursor.hot.x = 0;
    cursor.hot.y = 0;

    // Apply cursor settings
    if (ioctl(fd, WSDISPLAYIO_SCURSOR, &cursor) == -1)
        err(1, "Cannot set cursor");

    printf("Cursor set to a white block. Press Enter to exit.\n");
    getchar();

    // Clean up
    close(fd);

    return 0;
}
