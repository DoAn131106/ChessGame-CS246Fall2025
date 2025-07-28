#ifndef INFO_H
#define INFO_H
#include <cstddef>

class Piece;
struct Info {
  size_t row, col;
  Piece* piece; // Points to the Piece currently occupying the cell.
};

#endif
