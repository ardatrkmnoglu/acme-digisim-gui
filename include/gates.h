#ifndef GATES_H
#define GATES_H

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

void draw_and_gate(Display* dpy, Window w, GC gc, int size, int x, int y);

void draw_nand_gate(Display* dpy, Window w, GC gc, int size, int x, int y);

void draw_or_gate(Display* dpy, Window w, GC gc, int size, int x, int y);

void draw_nor_gate(Display* dpy, Window w, GC gc, int size, int x, int y);

void draw_xor_gate(Display* dpy, Window w, GC gc, int size, int x, int y);

void draw_xnor_gate(Display* dpy, Window w, GC gc, int size, int x, int y);

void draw_buffer(Display* dpy, Window w, GC gc, int size, int x, int y);

void draw_not_gate(Display* dpy, Window w, GC gc, int size, int x, int y);

void draw_threestate_buffer(Display* dpy, Window w, GC gc, int size, int x, int y);

#endif
