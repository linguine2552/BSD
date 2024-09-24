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
    struct wsdisplay_fbinfo fbinfo;

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

    printf("Getting framebuffer info...\n");
    if (ioctl(fd, WSDISPLAYIO_GINFO, &fbinfo) == -1) {
        print_error("Cannot get framebuffer info");
        close(fd);
        return 1;
    }

    printf("Framebuffer info:\n");
    printf("  Height: %d\n", fbinfo.height);
    printf("  Width: %d\n", fbinfo.width);
    printf("  Depth: %d\n", fbinfo.depth);
    printf("  CMSIZE: %d\n", fbinfo.cmsize);

    printf("All operations completed successfully. Press Enter to exit.\n");
    getchar();

    close(fd);
    return 0;
}
