#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <time.h>  // Add this for struct timespec

// If these are not defined in your system's wsconsio.h, define them here
#ifndef WSDISPLAYIO_GETCAPS
#define WSDISPLAYIO_GETCAPS _IOR('W', 100, unsigned long)
#endif

#ifndef WSDISPLAY_CAP_EMUL_CURSOR
#define WSDISPLAY_CAP_EMUL_CURSOR 0x00000001
#define WSDISPLAY_CAP_CURSOR_BLINK 0x00000002
#define WSDISPLAY_CAP_SWAP_CURSOR 0x00000004
#define WSDISPLAY_CAP_CURSOR_CLAMP 0x00000008
#endif

#include <dev/wscons/wsconsio.h>

#define CONSOLE "/dev/ttyC0"

void print_error(const char *msg) {
    printf("%s: %s\n", msg, strerror(errno));
}

int main() {
    int fd;
    unsigned long caps;

    printf("Opening console...\n");
    fd = open(CONSOLE, O_RDWR);
    if (fd == -1) {
        print_error("Cannot open console");
        return 1;
    }

    printf("Getting display capabilities...\n");
    if (ioctl(fd, WSDISPLAYIO_GETCAPS, &caps) == -1) {
        print_error("Cannot get display capabilities");
        close(fd);
        return 1;
    }

    printf("Display capabilities:\n");
    printf("  WSDISPLAY_CAP_EMUL_CURSOR: %s\n", (caps & WSDISPLAY_CAP_EMUL_CURSOR) ? "Yes" : "No");
    printf("  WSDISPLAY_CAP_CURSOR_BLINK: %s\n", (caps & WSDISPLAY_CAP_CURSOR_BLINK) ? "Yes" : "No");
    printf("  WSDISPLAY_CAP_SWAP_CURSOR: %s\n", (caps & WSDISPLAY_CAP_SWAP_CURSOR) ? "Yes" : "No");
    printf("  WSDISPLAY_CAP_CURSOR_CLAMP: %s\n", (caps & WSDISPLAY_CAP_CURSOR_CLAMP) ? "Yes" : "No");

    printf("All operations completed successfully. Press Enter to exit.\n");
    getchar();

    close(fd);
    return 0;
}
