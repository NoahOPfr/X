#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>

static unsigned long color(Display *display, int screen, const char *name) {
    XColor exact;
    XColor visual;
    Colormap colormap = DefaultColormap(display, screen);

    if (XAllocNamedColor(display, colormap, name, &visual, &exact)) {
        return visual.pixel;
    }

    return BlackPixel(display, screen);
}

static void draw_scene(Display *display, Window window, int screen,
                       int width, int height) {
    GC graphics = XCreateGC(display, window, 0, NULL);
    unsigned long dark_blue = color(display, screen, "#102a4c");

    XSetForeground(display, graphics, dark_blue);
    XFillRectangle(display, window, graphics, 0, 0, width, height);

    XFreeGC(display, graphics);
}

int main(void) {
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Unable to open the X display.\n");
        return 1;
    }

    int screen = DefaultScreen(display);
    int window_width = 640;
    int window_height = 400;
    Window window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        100,
        100,
        640,
        400,
        1,
        BlackPixel(display, screen),
        WhitePixel(display, screen));

    XStoreName(display, window, "My X11 Window");
    XSelectInput(display, window, ExposureMask | StructureNotifyMask);

    Atom close_atom = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &close_atom, 1);
    XMapWindow(display, window);

    int running = 1;
    while (running) {
        XEvent event;
        XNextEvent(display, &event);

        if (event.type == Expose) {
            draw_scene(display, window, screen, window_width, window_height);
        } else if (event.type == ConfigureNotify) {
            window_width = event.xconfigure.width;
            window_height = event.xconfigure.height;
            draw_scene(display, window, screen, window_width, window_height);
        } else if (event.type == ClientMessage &&
                   (Atom)event.xclient.data.l[0] == close_atom) {
            running = 0;
        }
    }

    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
