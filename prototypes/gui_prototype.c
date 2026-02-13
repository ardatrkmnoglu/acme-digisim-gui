#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define GRID_SIZE 20
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

unsigned long create_color(Display* dpy, int s, int r, int g, int b) {
	XColor* color;
	color->red = r * 256;
	color->green = g * 256;
	color->blue = b * 256;
	color->flags = DoRed | DoGreen | DoBlue;
	XAllocColor(dpy, DefaultColormap(dpy, s), color);
	return color->pixel;
}


