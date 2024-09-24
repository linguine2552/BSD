#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <time.h>

#define HOST_NAME_MAX 255
#define CPUSTATES 5
#define BAR_WIDTH 100

// Define CPU states
#define CP_USER   0
#define CP_NICE   1
#define CP_SYS    2
#define CP_SPIN   3
#define CP_IDLE   4

typedef unsigned long long cptime_t;

void get_cpu_usage(float *usage) {
    static cptime_t old_cp_time[CPUSTATES];
    cptime_t cp_time[CPUSTATES], total, diff[CPUSTATES];
    size_t size = sizeof(cp_time);
    int i, mib[2] = {CTL_KERN, KERN_CPTIME};

    if (sysctl(mib, 2, &cp_time, &size, NULL, 0) < 0) {
        perror("sysctl");
        exit(1);
    }

    total = 0;
    for (i = 0; i < CPUSTATES; i++) {
        diff[i] = cp_time[i] - old_cp_time[i];
        total += diff[i];
    }

    *usage = total > 0 ? 100.0 - (100.0 * diff[CP_IDLE] / total) : 0;

    memcpy(old_cp_time, cp_time, sizeof(cp_time));
}

void print_bar(float percentage, int width) {
    int filled = (int)(percentage / 100.0 * width);
    printf("[");
    for (int i = 0; i < width; i++) {
        if (i < filled) {
            printf("|");
        } else {
            printf(" ");
        }
    }
    printf("] %.1f%%\n", percentage);
}

int main() {
    char hostname[HOST_NAME_MAX];
    struct timeval boottime, now;
    time_t uptime;
    int mib[2] = {CTL_KERN, KERN_BOOTTIME};
    size_t size = sizeof(boottime);
    float cpu_usage;

    // Get hostname
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("Error getting hostname");
        return 1;
    }

    while (1) {
        // Get boot time
        if (sysctl(mib, 2, &boottime, &size, NULL, 0) != 0) {
            perror("Error getting boot time");
            return 1;
        }

        // Get current time
        if (gettimeofday(&now, NULL) != 0) {
            perror("Error getting current time");
            return 1;
        }

        // Calculate uptime
        uptime = now.tv_sec - boottime.tv_sec;

        // Calculate hours, minutes, and seconds
        int hours = uptime / 3600;
        int minutes = (uptime % 3600) / 60;
        int seconds = uptime % 60;

        // Get CPU usage
        get_cpu_usage(&cpu_usage);

        // Print hostname, uptime, and CPU usage
        printf("\033[2J\033[H"); // Clear screen and move cursor to top-left
        printf("Hostname: %s\n", hostname);
        printf("Uptime: %02d:%02d:%02d\n", hours, minutes, seconds);
        printf("CPU ");
        print_bar(cpu_usage, BAR_WIDTH);

        sleep(1); // Wait for 1 second before updating
    }

    return 0;
}
