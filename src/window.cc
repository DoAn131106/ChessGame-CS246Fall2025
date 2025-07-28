#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include "window.h"

using namespace std;

Xwindow::Xwindow(int width, int height) : width{width}, height{height} {
  d = XOpenDisplay(NULL);
  if (d == NULL) {
    cerr << "Cannot open display" << endl;
    exit(1);
  }
  s = DefaultScreen(d);
  w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, width, height, 1,
                          BlackPixel(d, s), WhitePixel(d, s));
  XSelectInput(d, w, ExposureMask | KeyPressMask | ButtonPressMask);
  XMapRaised(d, w);

  Pixmap pix = XCreatePixmap(d, w, width, height, DefaultDepth(d, DefaultScreen(d)));
  gc = XCreateGC(d, pix, 0, (XGCValues *)0);

  XFlush(d);
  XFlush(d);

  // Set up colours.
  XColor xcolour;
  Colormap cmap;
  char color_vals[6][10] = {"white", "black", "red", "green", "blue", "sienna"};

  cmap = DefaultColormap(d, DefaultScreen(d));
  for (int i = 0; i < 6; ++i) {
    XParseColor(d, cmap, color_vals[i], &xcolour);
    XAllocColor(d, cmap, &xcolour);
    colours[i] = xcolour.pixel;
  }

  XSetForeground(d, gc, colours[Black]);

  // Make window non-resizable.
  XSizeHints hints;
  hints.flags = (USPosition | PSize | PMinSize | PMaxSize);
  hints.height = hints.base_height = hints.min_height = hints.max_height = height;
  hints.width = hints.base_width = hints.min_width = hints.max_width = width;
  XSetNormalHints(d, w, &hints);

  XSynchronize(d, True);
  usleep(1000);
}

Xwindow::~Xwindow() {
  XFreeGC(d, gc);
  XCloseDisplay(d);
}

int Xwindow::getWidth() const { return width; }
int Xwindow::getHeight() const { return height; }

void Xwindow::fillRectangle(int x, int y, int width, int height, int colour) {
  XSetForeground(d, gc, colours[colour]);
  XFillRectangle(d, w, gc, x, y, width, height);
  XSetForeground(d, gc, colours[Black]);
}

void Xwindow::drawPixmap(int x, int y, const unsigned char *bitmap, int width, int height, int squareColor,
                         int squareSize, int colourpiece) {
  Pixmap pm = XCreateBitmapFromData(d, w, (char *)bitmap, width, height);
  XSetForeground(d, gc, colours[colourpiece]);  // The color for the "1" bits
  XSetBackground(d, gc, colours[squareColor]); 
  XCopyPlane(d, pm, w, gc, 0, 0, width, height, x, y, 1);
  XFreePixmap(d, pm);
}


void Xwindow::drawString(int x, int y, string msg, int colour) {
  XSetForeground(d, gc, colours[colour]);
  XDrawString(d, w, gc, x, y, msg.c_str(), msg.length());
  XSetForeground(d, gc, colours[Black]);
}
