#ifndef MENU_H
#define MENU_H

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

unsigned long create_color(Display* dpy, int s, int r, int g, int b);

void draw_toolbar(Display *dpy, Window w, GC gc, int width);

void draw_grid(Display *d, Window w, GC gc);

#endif
