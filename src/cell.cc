#include "cell.h"
#include "piece.h"
#include <utility>

// Constructor
Cell::Cell(size_t r, size_t c) : r{r}, c{c}, piece{nullptr} {}

Cell::Cell(const Cell& other) : r(other.r), c(other.c) {
    if (other.piece) {
        piece = other.piece->clone();
    }
}
// Sets a piece of the given colour on the cell.
void Cell::setPiece(std::unique_ptr<Piece> p) {
    piece = std::move(p);
    notifyObservers();
}

std::unique_ptr<Piece> Cell::removePiece() {
    auto oldPiece = std::move(piece);
    piece = nullptr;
    notifyObservers();
    return oldPiece;
}

std::unique_ptr<Piece> & Cell::getPiecePtr() {
    return piece;  // your internal member variable storing the piece
}

Piece* Cell::getPiece() const {
    return piece.get();
}

// Returns info about this cell including its row, column, and colour.
Info Cell::getInfo() const {
    return Info{r, c, piece.get()};
}
