#ifndef _WINDOW_H_
#define _WINDOW_H_
#include <X11/Xlib.h>
#include <iostream>
#include <string>

class Xwindow {
  Display *d;
  Window w;
  int s, width, height;
  GC gc;
  unsigned long colours[6];

 public:
  Xwindow(int width=640, int height=640);  // Constructor; displays the window.
  ~Xwindow();                              // Destructor; destroys the window.

  enum {White=0, Black, Red, Green, Blue, Brown}; // Available colours.

  int getWidth() const;
  int getHeight() const;

  // Draws a rectangle
  void fillRectangle(int x, int y, int width, int height, int colour=Black);

  void drawPixmap(int x, int y, const unsigned char *bitmap, int width, int height, int squareColor, int squareSize, int colourpiece);


  // Draws a string with specified colour
  void drawString(int x, int y, std::string msg, int colour=Black);
};

#endif
