#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <stdio.h>  // Add this line for fprintf and stderr
#include <string.h>

int main() {
    Display *display;
    Window window;
    XEvent event;
    GC gc;
    int screen;

    // Open the display
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    screen = DefaultScreen(display);

    // Create the window
    window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                 10, 10, 400, 300, 1,
                                 BlackPixel(display, screen),
                                 WhitePixel(display, screen));

    // Select kinds of events we are interested in
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    // Create GC for drawing
    gc = XCreateGC(display, window, 0, NULL);

    // Show the window
    XMapWindow(display, window);

    // Event loop
    while (1) {
        XNextEvent(display, &event);

        if (event.type == Expose) {
            // Draw the green rectangle
            XSetForeground(display, gc, 0x00FF00);  // Green color
            XFillRectangle(display, window, gc, 50, 50, 300, 200);
        }

        if (event.type == KeyPress)
            break;
    }

    // Close the display
    XCloseDisplay(display);
    return 0;
}
