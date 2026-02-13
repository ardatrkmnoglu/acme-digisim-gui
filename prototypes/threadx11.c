#include <X11/X.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <unistd.h>


int main() {
	Display* display = XOpenDisplay(0);
	assert(display);

	int black = BlackPixel(display, DefaultScreen(display));
	int white = WhitePixel(display, DefaultScreen(display));

	Window win = XCreateSimpleWindow(display, DefaultRootWindow(display), 100, 100, 1000, 800, 0, white, black);

	XSelectInput(display, win, StructureNotifyMask);
	XMapWindow(display, win);

	GC gc = XCreateGC(display, win, 0, 0);
	XSetForeground(display, gc, black);

	XEvent* e;
	while (e->type != MapNotify)
		XNextEvent(display, e);

	sleep(10);

	return 0;
}
