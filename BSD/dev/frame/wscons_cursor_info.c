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
    struct wsdisplay_cursor cursor;

    printf("Opening console...\n");
    fd = open(CONSOLE, O_RDWR);
    if (fd == -1) {
        print_error("Cannot open console");
        return 1;
    }

    printf("Initializing cursor structure...\n");
    memset(&cursor, 0, sizeof(cursor));
    cursor.which = WSDISPLAY_CURSOR_DOALL;

    printf("Getting current cursor info...\n");
    if (ioctl(fd, WSDISPLAYIO_GCURSOR, &cursor) == -1) {
        print_error("Cannot get cursor info");
        close(fd);
        return 1;
    }

    printf("Cursor info:\n");
    printf("  Enable: %d\n", cursor.enable);
    printf("  Position: (%d, %d)\n", cursor.pos.x, cursor.pos.y);
    printf("  Hot spot: (%d, %d)\n", cursor.hot.x, cursor.hot.y);
    printf("  Size: %dx%d\n", cursor.size.x, cursor.size.y);

    printf("All operations completed successfully. Press Enter to exit.\n");
    getchar();

    close(fd);
    return 0;
}
