#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_PATH 256

void check_device(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        printf("Device: %s\n", path);
        printf("  Major: %d, Minor: %d\n", major(st.st_rdev), minor(st.st_rdev));
        
        int fd = open(path, O_RDWR);
        if (fd != -1) {
            printf("  Successfully opened device\n");
            close(fd);
        } else {
            perror("  Failed to open device");
        }
    } else {
        perror("stat");
    }
}

int main() {
    const char *dev_dirs[] = {"/dev", "/dev/dri", NULL};
    const char *dev_prefixes[] = {"tty", "wsdisplay", "drm", NULL};

    for (int i = 0; dev_dirs[i] != NULL; i++) {
        DIR *dir = opendir(dev_dirs[i]);
        if (dir == NULL) {
            printf("Could not open directory: %s\n", dev_dirs[i]);
            continue;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            for (int j = 0; dev_prefixes[j] != NULL; j++) {
                if (strncmp(entry->d_name, dev_prefixes[j], strlen(dev_prefixes[j])) == 0) {
                    char full_path[MAX_PATH];
                    snprintf(full_path, sizeof(full_path), "%s/%s", dev_dirs[i], entry->d_name);
                    check_device(full_path);
                }
            }
        }

        closedir(dir);
    }

    return 0;
}
