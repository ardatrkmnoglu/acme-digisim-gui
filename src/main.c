#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "../include/menu.h"
#include "../include/gates.h"

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

		if (e.type == ButtonPress && e.xbutton.button == Button1) {
			int x = e.xbutton.x;
			int y = e.xbutton.y;

			if (y < TOOLBAR_HEIGHT) {
				int clicked_index = x / BUTTON_WIDTH;

				if (clicked_index >= 0 && clicked_index < 4) {
					current_tool = (ToolType)clicked_index;
					fprintf(stdout, "[LOG] Toggle tool: %d\n", current_tool);
					XClearArea(display, win, 0, 0, WIN_WIDTH, TOOLBAR_HEIGHT, true);
				}
			} else {
				fprintf(stdout, "[LOG] Clicked on grid at position: (%d, %d) - Tool: %d\n", x, y, current_tool);
			}
		}

		if (e.type == Expose && e.xexpose.count == 0) {
			XSetForeground(display, gc, grid_color);
			draw_grid(display, win, gc);
			draw_toolbar(display, win, gc, WIN_WIDTH);

			XSetForeground(display, gc, gate_color);
			XSetLineAttributes(display, gc, 2, LineSolid, CapButt, JoinMiter);

			int start_y = TOOLBAR_HEIGHT + 50;
			int gate_size = 40;
			int col_spacing = 150;

			draw_and_gate(display, win, gc, gate_size, 50, start_y);
			draw_or_gate(display, win, gc, gate_size, 50 + col_spacing, start_y);
			draw_xor_gate(display, win, gc, gate_size, 50 + col_spacing * 2, start_y);

			int row2_y = start_y + 100;
			draw_nand_gate(display, win, gc, gate_size, 50, row2_y);
			draw_nor_gate(display, win, gc, gate_size, 50 + col_spacing, row2_y);
			draw_xnor_gate(display, win, gc, gate_size, 50 + col_spacing * 2, row2_y);

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

