#ifndef ROOK_H
#define ROOK_H

#include "piece.h"
#include "board.h"

class Rook : public Piece {
public:
    // Constructor: initialize Rook  with a specific colour
    explicit Rook (Colour colour);

    // Returns true if moving from (fromRow, fromCol) to (toRow, toCol) is a legal move for a Rook.
    bool isLegalMove(const Board &board, int fromRow, int fromCol, int toRow, int toCol) const override;

    // Returns the symbol representing the Rook.
    // 'R' for white, 'r' for black.
    char getSymbol() const override;
    PieceType getType() const;
    std::vector<Move> getLegalMoves(const Board &board, int fromRow, int fromCol) const;

    ~Rook() override = default; // default destructor

    std::unique_ptr<Piece> clone() const override;
};

#endif
