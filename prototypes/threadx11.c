#include <X11/X.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <unistd.h>


int main() {
	Display* display = XOpenDisplay(0);
	assert(display);

	int black = BlackPixel(display, DefaultScreen(display));
	int white = WhitePixel(display, DefaultScreen(display));

	Window w1 = XCreateSimpleWindow(display, DefaultRootWindow(display), 100, 100, 1000, 800, 0, white, black);
	Window w2 = XCreateSimpleWindow(display, DefaultRootWindow(display), 200, 150, 400, 300, 1, black, white);

	XSelectInput(display, w1, StructureNotifyMask);
	XMapWindow(display, w1);
	XSelectInput(display, w2, StructureNotifyMask);
	XMapWindow(display, w2);

	GC gc = XCreateGC(display, w1, 0, 0);
	XSetForeground(display, gc, black);
	GC gc2 = XCreateGC(display, w2, 0, 0);
	XSetForeground(display, gc2, white);

	XEvent* e;
	while (e->type != MapNotify)
		XNextEvent(display, e);

	sleep(10);

	return 0;
}
