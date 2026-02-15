#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <assert.h>
#include <stdbool.h>
#include "gates.h"

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

/* Function pointer for the gate functions -- to be used later. */
// void (*draw_unit)(Display* dpy, Window w, GC gc, int size, int x, int y);

/* Gate functions with some additional explanations and notes that I've taken. */
void draw_and_gate(Display* dpy, Window w, GC gc, int size, int x, int y) {
	int width = size;
	int mid_y = y + size / 2;

	// Rectangular partition (body) of our AND gate.
	XDrawLine(dpy, w, gc, x, y, x, y + size);
	XDrawLine(dpy, w, gc, x, y, x + width / 2, y);
	XDrawLine(dpy, w, gc, x, y + size, x + width / 2, y + size);

	// Circular partition (nose) of our gate.
	XDrawArc(dpy, w, gc, x, y, width, size, -90 * 64, 180 * 64);

	// The pins.
	XDrawLine(dpy, w, gc, x - 10, y + size / 4, x, y + size / 4);
	XDrawLine(dpy, w, gc, x - 10, y + 3 * size / 4, x, y + 3 * size / 4);
	XDrawLine(dpy, w, gc, x + width, mid_y, x + width + 10, mid_y);
}

void draw_nand_gate(Display* dpy, Window w, GC gc, int size, int x, int y) {
	int width = size;
	int mid_y = y + size / 2;
	int bubble_r = size / 6;

	XDrawLine(dpy, w, gc, x, y, x, y + size);
	XDrawLine(dpy, w, gc, x, y, x + width / 2, y);
	XDrawLine(dpy, w, gc, x, y + size, x + width / 2, y + size);

	XDrawArc(dpy, w, gc, x, y, width - (bubble_r * 2), size, -90 * 64, 180 * 64);

	// The bubble of our NAND gate.
	XDrawArc(dpy, w, gc, x + width - (bubble_r * 2), mid_y - bubble_r,
		 bubble_r * 2, bubble_r * 2, 0, 360 * 64);

	XDrawLine(dpy, w, gc, x - 10, y + size / 4, x, y + size / 4);
	XDrawLine(dpy, w, gc, x - 10, y + 3 * size / 4, x, y + 3 * size / 4);
	XDrawLine(dpy, w, gc, x + width, mid_y, x + width + 10, mid_y);
}

void draw_or_gate(Display* dpy, Window w, GC gc, int size, int x, int y) {
	int width = size;
	int mid_y = y + size / 2;
	int indent = size / 5;

	// The rear-arc of our OR gate.
	XDrawArc(dpy, w, gc, x - indent, y, indent * 2, size, -90 * 64, 180 * 64);

	// Remaining part of the body.
	XDrawLine(dpy, w, gc, x + size / 10, y, x + width / 2, y);
	XDrawLine(dpy, w, gc, x + width / 2, y, x + width, mid_y);

	XDrawLine(dpy, w, gc, x + size / 10, y + size, x + width / 2, y + size);
	XDrawLine(dpy, w, gc, x + width / 2, y + size, x + width, mid_y);

	XDrawLine(dpy, w, gc, x - 10, y + size / 4, x + size / 8, y + size / 4);
	XDrawLine(dpy, w, gc, x - 10, y + 3 * size / 4, x + size / 8, y + 3 * size / 4);
	XDrawLine(dpy, w, gc, x + width, mid_y, x + width + 10, mid_y);
}

void draw_nor_gate(Display* dpy, Window w, GC gc, int size, int x, int y) {
	int width = size;
	int mid_y = y + size / 2;
	int indent = size / 5;
	int bubble_r = size / 6;

	// The body of our NOR gate.
	XDrawArc(dpy, w, gc, x - indent, y, indent * 2, size, -90 * 64, 180 * 64);
	XDrawLine(dpy, w, gc, x + size / 10, y, x + width / 2, y);
	XDrawLine(dpy, w, gc, x + width / 2, y, x + width - bubble_r * 2, mid_y);

	XDrawLine(dpy, w, gc, x + size / 10, y + size, x + width / 2, y + size);
	XDrawLine(dpy, w, gc, x + width / 2, y + size, x + width - bubble_r * 2, mid_y);

	// The bubble of our gate.
	XDrawArc(dpy, w, gc, x + width - bubble_r * 2, mid_y - bubble_r,
		 bubble_r * 2, bubble_r * 2, 0, 360 * 64);

	XDrawLine(dpy, w, gc, x - 10, y + size / 4, x + size / 8, y + size / 4);
	XDrawLine(dpy, w, gc, x - 10, y + 3 * size / 4, x + size / 8, y + 3 * size / 4);
	XDrawLine(dpy, w, gc, x + width, mid_y, x + width + 10, mid_y);
}

void draw_xor_gate(Display* dpy, Window w, GC gc, int size, int x, int y) {
	int width = size;
	int mid_y = y + size / 2;
	int gap = 5;
	int indent = size / 5;

	// Double rear-arc of our XOR gate.
	XDrawArc(dpy, w, gc, x - indent - gap, y, indent * 2, size, -90 * 64, 180 * 64);
	XDrawArc(dpy, w, gc, x - indent, y, indent * 2, size, -90 * 64, 180 * 64);

	// Remaining part of the body.
	XDrawLine(dpy, w, gc, x + size / 10, y, x + width / 2, y);
	XDrawLine(dpy, w, gc, x + width / 2, y, x + width, mid_y);
	XDrawLine(dpy, w, gc, x + size / 10, y + size, x + width / 2, y + size);
	XDrawLine(dpy, w, gc, x + width / 2, y + size, x + width, mid_y);

	XDrawLine(dpy, w, gc, x - 10 - gap, y + size / 4, x - gap, y + size / 4);
	XDrawLine(dpy, w, gc, x - 10 - gap, y + 3 * size / 4, x - gap, y + 3 * size / 4);
	XDrawLine(dpy, w, gc, x + width, mid_y, x + width + 10, mid_y);
}

void draw_xnor_gate(Display* dpy, Window w, GC gc, int size, int x, int y) {
	int width = size;
	int mid_y = y + size / 2;
	int bubble_r = size / 6;
	int gap = 5;
	int indent = size / 5;

	// Double rear-arc of our XNOR gate.
	XDrawArc(dpy, w, gc, x - indent - gap, y, indent * 2, size, -90 * 64, 180 * 64);
	XDrawArc(dpy, w, gc, x - indent, y, indent * 2, size, -90 * 64, 180 * 64);

	// Remaining part of the body.
	XDrawLine(dpy, w, gc, x + size / 10, y, x + width / 2, y);
	XDrawLine(dpy, w, gc, x + width / 2, y, x + width - bubble_r * 2, mid_y);
	XDrawLine(dpy, w, gc, x + size / 10, y + size, x + width / 2, y + size);
	XDrawLine(dpy, w, gc, x + width / 2, y + size, x + width - bubble_r * 2, mid_y);

	// The bubble of our gate.
	XDrawArc(dpy, w, gc, x + width - bubble_r * 2, mid_y - bubble_r,
		 bubble_r * 2, bubble_r * 2, 0, 360 * 64);

	XDrawLine(dpy, w, gc, x - 10 - gap, y + size / 4, x - gap, y + size / 4);
	XDrawLine(dpy, w, gc, x - 10 - gap, y + 3 * size / 4, x - gap, y + 3 * size / 4);
	XDrawLine(dpy, w, gc, x + width, mid_y, x + width + 10, mid_y);
}

void draw_buffer(Display* dpy, Window w, GC gc, int size, int x, int y) {
	int width = size;
	int mid_y = y + size / 2;

	// The body of our buffer.
	XPoint points[4];
	points[0].x = x;
	points[0].y = y;            // Top-left corner.

	points[1].x = x;
	points[1].y = y + size;     // Bottom-left corner.

	points[2].x = x + width;
	points[2].y = mid_y;        // Right corner of the triangle body.

	points[3].x = x;
	points[3].y = y;            // Close the shape.

	XDrawLines(dpy, w, gc, points, 4, CoordModeOrigin);
}

void draw_not_gate(Display* dpy, Window w, GC gc, int size, int x, int y) {
	int width = size;
	int mid_y = y + size / 2;
	int bubble_r = size / 6;
	int triangle_end = x + width - (bubble_r * 2);

	// The body of our NOT gate.
	XPoint points[4];
	points[0].x = x;
	points[0].y = y;

	points[1].x = x;
	points[1].y = y + size;

	points[2].x = triangle_end;
	points[2].y = mid_y;

	points[3].x = x;
	points[3].y = y;

	XDrawLines(dpy, w, gc, points, 4, CoordModeOrigin);

	// The bubble at the end of the triangle body.
	XDrawArc(dpy, w, gc, triangle_end, mid_y - bubble_r,
		 bubble_r * 2, bubble_r * 2, 0, 360 * 64);

	XDrawLine(dpy, w, gc, x - 10, mid_y, x, mid_y);
	XDrawLine(dpy, w, gc, x + width, mid_y, x + width + 10, mid_y);
}

void draw_threestate_buffer(Display* dpy, Window w, GC gc, int size, int x, int y) {
	draw_buffer(dpy, w, gc, size, x, y);    // A regular buffer.

	// The line that differs from the regular buffer.
	int mid_x = x + size / 2;
	int mid_y = y + size / 2;
	XDrawLine(dpy, w, gc, mid_x, y + (size * 0.8), mid_x, y + size + 10);
}


