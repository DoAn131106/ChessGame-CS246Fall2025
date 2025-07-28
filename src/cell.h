#ifndef CELL_H
#define CELL_H
#include <cstddef>
#include <memory>
#include "subject.h" 
#include "info.h"    

class Piece;

class Cell : public Subject {
    const size_t r, c;
    std::unique_ptr<Piece> piece;

public:
    Cell(size_t r, size_t c);
    Cell(const Cell& other);
    void setPiece(std::unique_ptr<Piece> p);
    std::unique_ptr<Piece> removePiece();
    Piece* getPiece() const;
    std::unique_ptr<Piece> & getPiecePtr();
    Info getInfo() const override;
};

#endif

