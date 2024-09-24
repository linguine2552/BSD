#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <dev/pci/drm/drm.h>

#define DRI_DEVICE "/dev/drm0"

int main() {
    int fd = open(DRI_DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Cannot open DRI device");
        return 1;
    }

    struct drm_mode_card_res res;
    memset(&res, 0, sizeof(res));

    if (ioctl(fd, DRM_IOCTL_MODE_GETRESOURCES, &res) < 0) {
        perror("Cannot get DRM resources");
        close(fd);
        return 1;
    }

    uint32_t *connector_ids = malloc(res.count_connectors * sizeof(uint32_t));
    res.connector_id_ptr = (uintptr_t)connector_ids;

    if (ioctl(fd, DRM_IOCTL_MODE_GETRESOURCES, &res) < 0) {
        perror("Cannot get connector IDs");
        free(connector_ids);
        close(fd);
        return 1;
    }

    struct drm_mode_get_connector conn;
    memset(&conn, 0, sizeof(conn));

    for (int i = 0; i < res.count_connectors; i++) {
        conn.connector_id = connector_ids[i];
        if (ioctl(fd, DRM_IOCTL_MODE_GETCONNECTOR, &conn) < 0) {
            continue;
        }
        if (conn.connection == DRM_MODE_CONNECTED && conn.count_modes > 0) {
            break;
        }
    }

    if (conn.connection != DRM_MODE_CONNECTED) {
        fprintf(stderr, "No connected connector found\n");
        free(connector_ids);
        close(fd);
        return 1;
    }

    struct drm_mode_modeinfo *modes = malloc(conn.count_modes * sizeof(struct drm_mode_modeinfo));
    conn.modes_ptr = (uintptr_t)modes;

    if (ioctl(fd, DRM_IOCTL_MODE_GETCONNECTOR, &conn) < 0) {
        perror("Cannot get connector modes");
        free(modes);
        free(connector_ids);
        close(fd);
        return 1;
    }

    struct drm_mode_create_dumb create = {
        .width = modes[0].hdisplay,
        .height = modes[0].vdisplay,
        .bpp = 32
    };

    if (ioctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &create) < 0) {
        perror("Cannot create dumb buffer");
        free(modes);
        free(connector_ids);
        close(fd);
        return 1;
    }

    struct drm_mode_fb_cmd fb = {
        .width = create.width,
        .height = create.height,
        .pitch = create.pitch,
        .bpp = create.bpp,
        .depth = 24,
        .handle = create.handle
    };

    if (ioctl(fd, DRM_IOCTL_MODE_ADDFB, &fb) < 0) {
        perror("Cannot add framebuffer");
        free(modes);
        free(connector_ids);
        close(fd);
        return 1;
    }

    struct drm_mode_map_dumb map = {
        .handle = create.handle
    };

    if (ioctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &map) < 0) {
        perror("Cannot map dumb buffer");
        free(modes);
        free(connector_ids);
        close(fd);
        return 1;
    }

    void *fb_ptr = mmap(0, create.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, map.offset);
    if (fb_ptr == MAP_FAILED) {
        perror("Cannot mmap dumb buffer");
        free(modes);
        free(connector_ids);
        close(fd);
        return 1;
    }

    // Draw green rectangle
    uint32_t green = 0x0000FF00;  // ARGB format
    int rect_width = modes[0].hdisplay / 2;
    int rect_height = modes[0].vdisplay / 2;

    for (int y = 0; y < rect_height; y++) {
        for (int x = 0; x < rect_width; x++) {
            int index = (y * modes[0].hdisplay + x);
            ((uint32_t*)fb_ptr)[index] = green;
        }
    }

    struct drm_mode_crtc crtc = {
        .set_connectors_ptr = (uintptr_t)&conn.connector_id,
        .count_connectors = 1,
        .fb_id = fb.fb_id,
        .mode_valid = 1,
        .mode = modes[0]
    };

    if (ioctl(fd, DRM_IOCTL_MODE_SETCRTC, &crtc) < 0) {
        perror("Cannot set CRTC");
    } else {
        printf("Green rectangle drawn. Press Enter to exit...");
        getchar();
    }

    // Cleanup
    munmap(fb_ptr, create.size);
    struct drm_mode_destroy_dumb destroy = { .handle = create.handle };
    ioctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy);
    free(modes);
    free(connector_ids);
    close(fd);

    return 0;
}
