#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "menu.h"

#define GRID_SIZE 20
#define WIN_WIDTH 1200
#define WIN_HEIGHT 900
#define TOOLBAR_HEIGHT 60
#define BUTTON_WIDTH 80

typedef enum {
	TOOL_SELECT,
	TOOL_WIRE,
	TOOL_AND,
	TOOL_OR
} ToolType;
ToolType current_tool = TOOL_SELECT;

unsigned long create_color(Display* dpy, int s, int r, int g, int b) {
	XColor color;
	color.red = r * 256;
	color.green = g * 256;
	color.blue = b * 256;
	color.flags = DoRed | DoGreen | DoBlue;
	XAllocColor(dpy, DefaultColormap(dpy, s), &color);
	return color.pixel;
}

void draw_toolbar(Display *dpy, Window w, GC gc, int width) {
	XSetForeground(dpy, gc, 0xdddddd);
	XFillRectangle(dpy, w, gc, 0, 0, width, TOOLBAR_HEIGHT);

	XSetForeground(dpy, gc, 0x000000);
	XDrawLine(dpy, w, gc, 0, TOOLBAR_HEIGHT, width, TOOLBAR_HEIGHT);

	const char* tool_names[] = {"SELECT", "WIRE", "AND", "OR"};
	int num_tools = 4;

	for (int i = 0; i < num_tools; i++) {
		int x = i * BUTTON_WIDTH;

		if (i == current_tool) {
			XSetForeground(dpy, gc, 0xaaaaaa);
			XFillRectangle(dpy, w, gc, x, 0, BUTTON_WIDTH, TOOLBAR_HEIGHT);
		}

		XSetForeground(dpy, gc, 0x000000);
		XDrawRectangle(dpy, w, gc, x, 0, BUTTON_WIDTH, TOOLBAR_HEIGHT);
		XDrawString(dpy, w, gc, x + 20, 35, tool_names[i], strlen(tool_names[i]));    // 20 and 35 defines the
                                                                                              // font size here.

	}
}

void draw_grid(Display *d, Window w, GC gc) {
	for (int y = TOOLBAR_HEIGHT; y < WIN_HEIGHT; y += GRID_SIZE) {
		for (int x = 0; x < WIN_WIDTH; x += GRID_SIZE) {
			XDrawPoint(d, w, gc, x, y);
		}
	}
}

