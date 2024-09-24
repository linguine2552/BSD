#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_PROPERTY_VALUE_LEN 4096

// Color definitions
#define COLOR_DEFAULT 0x808080  // Gray
#define COLOR_TERMINAL 0x00FF00 // Green
#define COLOR_BROWSER 0x0000FF  // Blue
#define COLOR_EDITOR 0xFF0000   // Red
#define COLOR_I3 0xFFA500       // Orange for i3-related windows

// Debug logging function
void log_debug(const char* message) {
    time_t now;
    time(&now);
    char* date = ctime(&now);
    date[strlen(date) - 1] = '\0';  // Remove newline
    fprintf(stderr, "[%s] %s\n", date, message);
}

// Function to get the window class
static int get_window_class(Display *display, Window window, char *class_name, size_t class_name_len) {
    XClassHint class_hint;
    if (XGetClassHint(display, window, &class_hint)) {
        strncpy(class_name, class_hint.res_class, class_name_len - 1);
        class_name[class_name_len - 1] = '\0';
        XFree(class_hint.res_name);
        XFree(class_hint.res_class);
        return 1;
    }
    return 0;
}

// Function to set the window border color
static void set_window_border_color(Display *display, Window window, unsigned long color) {
    XSetWindowBorder(display, window, color);
    char debug_msg[256];
    snprintf(debug_msg, sizeof(debug_msg), "Set border color 0x%lX for window 0x%lX", color, window);
    log_debug(debug_msg);
}

// Function to process a window
static void process_window(Display *display, Window window) {
    char class_name[256];
    if (get_window_class(display, window, class_name, sizeof(class_name))) {
        unsigned long color = COLOR_DEFAULT;

        if (strstr(class_name, "XTerm") || strstr(class_name, "URxvt")) {
            color = COLOR_TERMINAL;
        } else if (strstr(class_name, "Firefox") || strstr(class_name, "Chromium")) {
            color = COLOR_BROWSER;
        } else if (strstr(class_name, "Vim") || strstr(class_name, "Emacs")) {
            color = COLOR_EDITOR;
        } else if (strstr(class_name, "i3")) {
            color = COLOR_I3;
        }

        set_window_border_color(display, window, color);

        char debug_msg[256];
        snprintf(debug_msg, sizeof(debug_msg), "Processed window 0x%lX with class '%s'", window, class_name);
        log_debug(debug_msg);
    } else {
        char debug_msg[256];
        snprintf(debug_msg, sizeof(debug_msg), "Failed to get window class for window 0x%lX", window);
        log_debug(debug_msg);
    }
}

// Function to process all windows
static void process_all_windows(Display *display, Window root) {
    Window root_return, parent_return, *children;
    unsigned int nchildren;

    if (XQueryTree(display, root, &root_return, &parent_return, &children, &nchildren)) {
        for (unsigned int i = 0; i < nchildren; i++) {
            process_window(display, children[i]);
        }
        XFree(children);
    }
}

// Error handler to prevent crashes
int error_handler(Display *display, XErrorEvent *error) {
    char error_msg[256];
    XGetErrorText(display, error->error_code, error_msg, sizeof(error_msg));
    log_debug(error_msg);
    return 0;
}

int main() {
    log_debug("Window border colorizer started");

    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        log_debug("Cannot open display");
        return 1;
    }

    // Set custom error handler
    XSetErrorHandler(error_handler);

    Window root = DefaultRootWindow(display);

    // Initial processing of all windows
    process_all_windows(display, root);

    // Set up event listening for new windows
    XSelectInput(display, root, SubstructureNotifyMask);

    XEvent event;
    while (1) {
        XNextEvent(display, &event);
        if (event.type == CreateNotify) {
            log_debug("New window created");
            // Add a small delay to allow the window to initialize
            usleep(50000);  // 50ms delay
            process_window(display, event.xcreatewindow.window);
        }
    }

    XCloseDisplay(display);
    return 0;
}
