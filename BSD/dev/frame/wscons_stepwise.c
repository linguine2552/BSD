#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <string.h>
#include <time.h>
#include <dev/wscons/wsconsio.h>
#include <sys/ioctl.h>
#include <errno.h>

#define CONSOLE "/dev/ttyC0"

void print_error(const char *msg) {
    printf("%s: %s\n", msg, strerror(errno));
}

int main() {
    int fd;
    int mode;
    struct wsdisplay_cursor cursor;

    printf("Opening console...\n");
    fd = open(CONSOLE, O_RDWR);
    if (fd == -1) {
        print_error("Cannot open console");
        return 1;
    }

    printf("Getting current mode...\n");
    if (ioctl(fd, WSDISPLAYIO_GMODE, &mode) == -1) {
        print_error("Cannot get mode");
        close(fd);
        return 1;
    }
    printf("Current mode: %d\n", mode);

    printf("Initializing cursor structure...\n");
    memset(&cursor, 0, sizeof(cursor));
    cursor.which = WSDISPLAY_CURSOR_DOCMAP;

    printf("Setting cursor color...\n");
    cursor.cmap.red[0] = cursor.cmap.green[0] = cursor.cmap.blue[0] = 0;
    cursor.cmap.red[1] = cursor.cmap.green[1] = cursor.cmap.blue[1] = 255;
    cursor.cmap.index = 0;
    cursor.cmap.count = 2;

    printf("Getting current cursor...\n");
    if (ioctl(fd, WSDISPLAYIO_GCURSOR, &cursor) == -1) {
        print_error("Cannot get cursor");
        close(fd);
        return 1;
    }

    printf("Applying cursor settings...\n");
    if (ioctl(fd, WSDISPLAYIO_SCURSOR, &cursor) == -1) {
        print_error("Cannot set cursor");
        close(fd);
        return 1;
    }

    printf("All operations completed successfully. Press Enter to exit.\n");
    getchar();

    close(fd);
    return 0;
}
