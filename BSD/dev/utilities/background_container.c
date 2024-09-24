#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    Display* display;
    Window window;
    XEvent event;
    int screen;
    char* window_name = "Background Container";

    // Open connection to X server
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    screen = DefaultScreen(display);

    // Get screen dimensions
    int width = DisplayWidth(display, screen);
    int height = DisplayHeight(display, screen);

    // Create the window
    window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                 0, 0, width, height, 0,
                                 BlackPixel(display, screen),
                                 BlackPixel(display, screen));

    // Set window properties
    XSetWindowAttributes attributes;
    attributes.override_redirect = True;
    XChangeWindowAttributes(display, window, CWOverrideRedirect, &attributes);

    // Set window name
    XStoreName(display, window, window_name);

    // Set window type to desktop
    Atom net_wm_window_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
    Atom net_wm_window_type_desktop = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DESKTOP", False);
    XChangeProperty(display, window, net_wm_window_type, XA_ATOM, 32, 
                    PropModeReplace, (unsigned char *) &net_wm_window_type_desktop, 1);

    // Select kind of events we are interested in
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    // Map (show) the window
    XMapWindow(display, window);

    // Event loop
    while (1) {
        XNextEvent(display, &event);

        // Draw or redraw the window
        if (event.type == Expose) {
            XFillRectangle(display, window, DefaultGC(display, screen),
                           0, 0, width, height);
        }

        // Exit on keypress
        if (event.type == KeyPress)
            break;
    }

    // Close connection to X server
    XCloseDisplay(display);

    return 0;
}
