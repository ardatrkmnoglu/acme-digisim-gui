#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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

/* @brief Toolbar function. */
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

/* @brief Background grid function. */
void draw_grid(Display *d, Window w, GC gc) {
	for (int y = TOOLBAR_HEIGHT; y < WIN_HEIGHT; y += GRID_SIZE) {
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
	win = XCreateSimpleWindow(display, XDefaultRootWindow(display),
				  100, 100, WIN_WIDTH, WIN_HEIGHT, 1,
				  BlackPixel(display, screen),
				  WhitePixel(display, screen));

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
		/* TEST #1
		if (e.type == Expose) {
			XSetForeground(display, gc, grid_color);
			draw_grid(display, win, gc);

			XSetForeground(display, gc, gate_color);
			XSetLineAttributes(display, gc, 2, LineSolid, CapButt, JoinMiter);
		}
		*/

		// TEST #2
		if (e.type == ButtonPress && e.xbutton.button == Button1) {
			int x = e.xbutton.x;
			int y = e.xbutton.y;

			if (y < TOOLBAR_HEIGHT) {
				int clicked_index = x / BUTTON_WIDTH;

				if (clicked_index >= 0 && clicked_index < 4) {
					current_tool = (ToolType)clicked_index;
					fprintf(stdout, "[LOG] Toggle tool: %d\n", current_tool);

					// Sadece toolbar alanını yeniden çizmek için o bölgeyi temizle
					// Bu işlem otomatik olarak bir Expose event'i tetikler.
					XClearArea(display, win, 0, 0, WIN_WIDTH, TOOLBAR_HEIGHT, true);
				}
			} else {
				fprintf(stdout, "[LOG] Clicked on grid at position: (%d, %d) - Tool: %d\n", x, y, current_tool);
			}
		}

		if (e.type == Expose && e.xexpose.count == 0) {
			// 1. Grid Çiz
			XSetForeground(display, gc, grid_color);
			draw_grid(display, win, gc);

			// 2. Toolbar Çiz (Grid'in üstüne basmalı)
			// draw_toolbar kendi içinde renkleri ayarlıyor, o yüzden burada renk set etmeye gerek yok.
			draw_toolbar(display, win, gc, WIN_WIDTH);

			// 3. Kapıları Test Etmek İçin Çiz (Katalog)
			XSetForeground(display, gc, gate_color);
			XSetLineAttributes(display, gc, 2, LineSolid, CapButt, JoinMiter);

			// Test çizimlerini biraz aşağıdan başlatalım (Toolbar'ın altından)
			int start_y = TOOLBAR_HEIGHT + 50;
			int gate_size = 40;
			int col_spacing = 150;

			// Satır 1: Temel Kapılar
			draw_and_gate(display, win, gc, gate_size, 50, start_y);

			draw_or_gate(display, win, gc, gate_size, 50 + col_spacing, start_y);

			draw_xor_gate(display, win, gc, gate_size, 50 + col_spacing * 2, start_y);

			// Satır 2: Türetilmiş Kapılar
			int row2_y = start_y + 100;
			draw_nand_gate(display, win, gc, gate_size, 50, row2_y);

			draw_nor_gate(display, win, gc, gate_size, 50 + col_spacing, row2_y);

			draw_xnor_gate(display, win, gc, gate_size, 50 + col_spacing * 2, row2_y);

			// Satır 3: Buffer'lar
			int row3_y = row2_y + 100;
			draw_buffer(display, win, gc, gate_size, 50, row3_y);

			draw_threestate_buffer(display, win, gc, gate_size, 50 + col_spacing, row3_y);

			draw_not_gate(display, win, gc, gate_size, 50 + col_spacing * 2, row3_y);
		}
	}

	XFreeGC(display, gc);
	XDestroyWindow(display, win);
	XCloseDisplay(display);
	return 0;
}

