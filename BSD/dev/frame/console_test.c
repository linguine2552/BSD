#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>

#define CONSOLE "/dev/ttyC0"

int main() {
    int fd;
    char c = '*';

    printf("Opening console...\n");
    fd = open(CONSOLE, O_RDWR);
    if (fd == -1)
        err(1, "Cannot open console");

    printf("Console opened. Attempting to write...\n");
    if (write(fd, &c, 1) == -1)
        err(1, "Cannot write to console");

    printf("Write successful. Press Enter to exit.\n");
    getchar();

    close(fd);
    return 0;
}
