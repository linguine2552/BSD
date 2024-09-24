#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <dev/wscons/wsconsio.h>

#define CONSOLE "/dev/ttyC0"

struct fb_info {
    int width;
    int height;
    int depth;
    int pitch;
    void *buffer;
    size_t buffer_size;
};

int get_fb_info(int fd, struct fb_info *info) {
    struct wsdisplay_fbinfo fb;
    if (ioctl(fd, WSDISPLAYIO_GINFO, &fb) == -1) {
        perror("Cannot get framebuffer info");
        return -1;
    }
    info->width = fb.width;
    info->height = fb.height;
    info->depth = fb.depth;
    info->pitch = fb.stride;
    info->buffer_size = info->pitch * info->height;
    return 0;
}

int map_framebuffer(int fd, struct fb_info *info) {
    info->buffer = mmap(NULL, info->buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (info->buffer == MAP_FAILED) {
        perror("Cannot map framebuffer");
        return -1;
    }
    return 0;
}

void unmap_framebuffer(struct fb_info *info) {
    if (info->buffer) {
        munmap(info->buffer, info->buffer_size);
        info->buffer = NULL;
    }
}

void draw_pixel(struct fb_info *info, int x, int y, unsigned int color) {
    if (x < 0 || x >= info->width || y < 0 || y >= info->height) {
        return;  // Out of bounds
    }
    unsigned int *pixel = (unsigned int *)((char *)info->buffer + y * info->pitch + x * (info->depth / 8));
    *pixel = color;
}

void draw_line(struct fb_info *info, int x1, int y1, int x2, int y2, unsigned int color) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while (1) {
        draw_pixel(info, x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = err;
        if (e2 > -dx) { err -= dy; x1 += sx; }
        if (e2 < dy) { err += dx; y1 += sy; }
    }
}

void draw_rectangle(struct fb_info *info, int x, int y, int width, int height, unsigned int color, int filled) {
    if (filled) {
        for (int i = y; i < y + height; i++) {
            for (int j = x; j < x + width; j++) {
                draw_pixel(info, j, i, color);
            }
        }
    } else {
        draw_line(info, x, y, x + width - 1, y, color);
        draw_line(info, x, y + height - 1, x + width - 1, y + height - 1, color);
        draw_line(info, x, y, x, y + height - 1, color);
        draw_line(info, x + width - 1, y, x + width - 1, y + height - 1, color);
    }
}

void clear_screen(struct fb_info *info, unsigned int color) {
    for (int y = 0; y < info->height; y++) {
        for (int x = 0; x < info->width; x++) {
            draw_pixel(info, x, y, color);
        }
    }
}

int main() {
    int fd = open(CONSOLE, O_RDWR);
    if (fd == -1) {
        perror("Cannot open console");
        return 1;
    }

    struct fb_info info;
    if (get_fb_info(fd, &info) == -1 || map_framebuffer(fd, &info) == -1) {
        close(fd);
        return 1;
    }

    // Clear screen to black
    clear_screen(&info, 0x000000);

    // Draw green border
    draw_rectangle(&info, 0, 0, info.width, info.height, 0x00FF00, 0);

    // Draw some sample shapes
    draw_rectangle(&info, 50, 50, 200, 100, 0xFF0000, 1);  // Filled red rectangle
    draw_line(&info, 300, 100, 500, 300, 0x0000FF);  // Blue diagonal line

    // Keep the drawing visible for 5 seconds
    sleep(5);

    // Clear screen to black before exiting
    clear_screen(&info, 0x000000);

    unmap_framebuffer(&info);
    close(fd);
    return 0;
}
