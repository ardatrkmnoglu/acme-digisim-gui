#include <X11/X.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <unistd.h>

int main() {
	
	Display* display = XOpenDisplay(0);
	assert(display);

	int white = WhitePixel(display, DefaultScreen(display));
	int black = BlackPixel(display, DefaultScreen(display));

	Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 100, 100, 1000, 800, 0, black, black);

	XSelectInput(display, window, StructureNotifyMask);

	XMapWindow(display, window);

	GC content = XCreateGC(display, window, 0, 0);

	XSetForeground(display, content, white);

	XEvent e;
	while (e.type != MapNotify)
		XNextEvent(display, &e);

	int x_1 = 100;
	int x_2 = 600;
	int y_1 = 180;
	int y_2 = 700;

	XDrawLine(display, window, content, x_1, x_2, y_1, y_2);
	XFlush(display);

	sleep(10);
}
