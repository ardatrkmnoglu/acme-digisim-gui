#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#define GRID_SIZE 20
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

unsigned long create_color(Display* dpy, int s, int r, int g, int b) {
	XColor color;
	color.red = r * 256;
	color.green = g * 256;
	color.blue = b * 256;
	color.flags = DoRed | DoGreen | DoBlue;
	XAllocColor(dpy, DefaultColormap(dpy, s), &color);
	return color.pixel;
}

void (*draw_unit)(Display* dpy, Window w, GC gc, int size, int x, int y);

void draw_grid(Display *d, Window w, GC gc) {
	for (int y = 0; y < WIN_HEIGHT; y += GRID_SIZE) {
		for (int x = 0; x < WIN_WIDTH; x += GRID_SIZE) {
			XDrawPoint(d, w, gc, x, y);
		}
	}
}

int main() {
	Display* display;
	Window win;
	XEvent e;
	int screen;

	display = XOpenDisplay(0);
	screen = XDefaultScreen(display);
	win = XCreateSimpleWindow(display, XDefaultRootWindow(display), 100, 100, 1200, 900, 1, BlackPixel(display, screen), WhitePixel(display, screen));

	XStoreName(display, win, "ACME Digisim - v0.0.1 Prototype");

	Atom delete_window = XInternAtom(display, "WM_DELETE_WINDOW", false);
	XSetWMProtocols(display, win, &delete_window, 1);

	XSelectInput(display, win, (ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask));
	XMapWindow(display, win);
	GC gc = XCreateGC(display, win, 0, 0);

	int grid_color = create_color(display, screen, 200, 200, 200);
	int gate_color = create_color(display, screen, 0, 0, 150);

	bool running = true;
	while (running) {
		XNextEvent(display, &e);

		if (e.type == ClientMessage) {
			if ((Atom)e.xclient.data.l[0] == delete_window) {
				fprintf(stdout, "[LOG] Got close signal from window manager.\n");
				fprintf(stdout, "[LOG] Exiting...\n");
				running = false;
			}
		}

		if (e.type == Expose) {
			XSetForeground(display, gc, grid_color);
			draw_grid(display, win, gc);

			XSetForeground(display, gc, gate_color);
			XSetLineAttributes(display, gc, 2, LineSolid, CapButt, JoinMiter);
		}
	}

	XFreeGC(display, gc);
	XDestroyWindow(display, win);
	XCloseDisplay(display);
	return 0;
}

