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
    struct wsdisplay_param param;

    printf("Opening console...\n");
    fd = open(CONSOLE, O_RDWR);
    if (fd == -1) {
        print_error("Cannot open console");
        return 1;
    }

    // Get brightness
    param.param = WSDISPLAYIO_PARAM_BRIGHTNESS;
    if (ioctl(fd, WSDISPLAYIO_GETPARAM, &param) == -1) {
        print_error("Cannot get brightness");
    } else {
        printf("Current brightness: %d\n", param.curval);
        printf("Min brightness: %d\n", param.min);
        printf("Max brightness: %d\n", param.max);
    }

    // Try to set brightness
    int new_brightness = (param.min + param.max) / 2;  // Set to middle value
    param.curval = new_brightness;
    if (ioctl(fd, WSDISPLAYIO_SETPARAM, &param) == -1) {
        print_error("Cannot set brightness");
    } else {
        printf("Brightness set to: %d\n", new_brightness);
    }

    // Get contrast
    param.param = WSDISPLAYIO_PARAM_CONTRAST;
    if (ioctl(fd, WSDISPLAYIO_GETPARAM, &param) == -1) {
        print_error("Cannot get contrast");
    } else {
        printf("Current contrast: %d\n", param.curval);
        printf("Min contrast: %d\n", param.min);
        printf("Max contrast: %d\n", param.max);
    }

    printf("Operations completed. Check if you notice any change in screen brightness.\n");
    printf("Press Enter to exit.\n");
    getchar();

    // Restore original brightness
    param.param = WSDISPLAYIO_PARAM_BRIGHTNESS;
    if (ioctl(fd, WSDISPLAYIO_GETPARAM, &param) == -1) {
        print_error("Cannot get original brightness");
    } else {
        if (ioctl(fd, WSDISPLAYIO_SETPARAM, &param) == -1) {
            print_error("Cannot restore original brightness");
        } else {
            printf("Original brightness restored.\n");
        }
    }

    close(fd);
    return 0;
}
