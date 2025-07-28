#include "graphicsdisplay.h"
#include "piece.h"
#include "subject.h"
#include <string>
using namespace std;

#include "Bwhite.xbm"
#include "Kwhite.xbm"
#include "Nwhite.xbm"
#include "Pwhite.xbm"
#include "Qwhite.xbm"
#include "Rwhite.xbm"
#include "bblack.xbm"
#include "kblack.xbm"
#include "nblack.xbm"
#include "pblack.xbm"
#include "qblack.xbm"
#include "rblack.xbm"

const int DEFAULT_WINDOW_SIZE = 400;
const int DEFAULT_WINDOW_4PLAYER = 700;

GraphicsDisplay::GraphicsDisplay(bool fourPlayer)
    : xw{fourPlayer ? DEFAULT_WINDOW_4PLAYER : DEFAULT_WINDOW_SIZE,
      fourPlayer ? DEFAULT_WINDOW_4PLAYER : DEFAULT_WINDOW_SIZE},
      gridSize{fourPlayer ? 14 : 8}, 
      windowSize{fourPlayer ? DEFAULT_WINDOW_4PLAYER : DEFAULT_WINDOW_SIZE},
      cellSize{fourPlayer ? DEFAULT_WINDOW_4PLAYER / 14 : DEFAULT_WINDOW_SIZE / 8} {
        for (int r = 0; r < gridSize; ++r) {
          for (int c = 0; c < gridSize; ++c) {
            int curCellRow = r * cellSize;
            int curCellCol = c * cellSize;
            int color;

            if (gridSize == 14 &&
            ((r < 3 && c < 3) ||                       // top-left
            (r < 3 && c >= gridSize - 3) ||           // top-right
            (r >= gridSize - 3 && c < 3) ||           // bottom-left
            (r >= gridSize - 3 && c >= gridSize - 3))) {  // bottom-right
              color = Xwindow::Black;
            } else {
              color = (r + c) % 2 == 0 ? Xwindow::White : Xwindow::Brown;
            }
            xw.fillRectangle(curCellCol, curCellRow, cellSize, cellSize, color);
          }
        }
  drawLabels();
}

void GraphicsDisplay::drawLabels() {
  for (int i = 0; i < gridSize; ++i) {
    // Row labels (left side)
    xw.drawString(2, i * cellSize + cellSize / 2, std::to_string(gridSize - i), Xwindow::Blue);

    // Column labels (bottom side)
    xw.drawString(i * cellSize + cellSize / 2, windowSize - 2, std::string(1, 'a' + i), Xwindow::Red);
  }
}


// graphicsdisplay.cc
void GraphicsDisplay::notify(Subject &whoFrom) {
  Info info = whoFrom.getInfo();
  int r = info.row, c = info.col;
  int x = c * cellSize, y = r * cellSize;

  // 1) redraw the background properly in White/Brown:
  int bgColor;
  if (gridSize == 14 && r < 3 && c < 3) {
    bgColor = Xwindow::Black;
  } else {
    bgColor = (r + c) % 2 == 0 ? Xwindow::White : Xwindow::Brown;
  }
  xw.fillRectangle(x, y, cellSize, cellSize, bgColor);

  // 2) pick & draw the correct XBM, centered:
  if (info.piece) {
    const unsigned char *bits = nullptr;
    int w = 0, h = 0;
    switch (info.piece->getSymbol()) {
    case 'p':
    case 'H':
    case 'h':
      bits = pblack_bits;
      w = pblack_width;
      h = pblack_height;
      break;
    case 'P':
      bits = pwhite_bits;
      w = pwhite_width;
      h = pwhite_height;
      break;
    case 'r':
    case 'D':
    case 'd':
      bits = rblack_bits;
      w = rblack_width;
      h = rblack_height;
      break;
    case 'R':
      bits = rwhite_bits;
      w = rwhite_width;
      h = rwhite_height;
      break;
    case 'n':
    case 'G':
    case 'g':
      bits = nblack_bits;
      w = nblack_width;
      h = nblack_height;
      break;
    case 'N':
      bits = nwhite_bits;
      w = nwhite_width;
      h = nwhite_height;
      break;
    case 'b':
    case 'F':
    case 'f':
      bits = bblack_bits;
      w = bblack_width;
      h = bblack_height;
      break;
    case 'B':
      bits = bwhite_bits;
      w = bwhite_width;
      h = bwhite_height;
      break;
    case 'q':
    case 'S':
    case 's':
      bits = qblack_bits;
      w = qblack_width;
      h = qblack_height;
      break;
    case 'Q':
      bits = qwhite_bits;
      w = qwhite_width;
      h = qwhite_height;
      break;
    case 'k':
    case 'A':
    case 'a':
      bits = kblack_bits;
      w = kblack_width;
      h = kblack_height;
      break;
    case 'K':
      bits = kwhite_bits;
      w = kwhite_width;
      h = kwhite_height;
      break;
    default:
      break;
    }
    int colourpiece;
    switch (info.piece->getSymbol()) {
    case 'B': case 'P': case 'N': case 'Q': case 'K': case 'R':
      colourpiece = Xwindow::Black;
      break;
    case 'b': case 'p': case 'n': case 'q': case 'k': case 'r':
      colourpiece = Xwindow::Black;
      break;
    case 'F': case 'H': case 'G': case 'S': case 'A': case 'D':
      colourpiece = Xwindow::Red;
      break;
    case 'f': case 'h': case 'g': case 's': case 'a': case 'd':
      colourpiece = Xwindow::Blue;
      break;
    }
    if (bits) {
      xw.drawPixmap(x, y, bits, w, h, bgColor, cellSize, colourpiece);
    }
  }
  if (c == 0) {
    xw.drawString(x + 2, y + cellSize / 2, std::to_string(gridSize - r), Xwindow::Blue);
  }
  if (r == gridSize - 1) {
      xw.drawString(x + cellSize / 2, windowSize - 2, std::string(1, 'a' + c), Xwindow::Red);
  }

}
