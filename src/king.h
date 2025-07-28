#ifndef KING_H
#define KING_H

#include "piece.h"
#include "board.h"

class King : public Piece {
public:
    // Constructor: initialize King with a specific colour
    explicit King(Colour colour);

    // Returns true if moving from (fromRow, fromCol) to (toRow, toCol) is a legal move for a King.
    bool isLegalMove(const Board &board, int fromRow, int fromCol, int toRow, int toCol) const override;

    // Returns the symbol representing the King.
    // 'K' for white, 'k' for black.
    char getSymbol() const override;

    ~King() override = default; // default destructor
    PieceType getType() const;
    std::vector<Move> getLegalMoves(const Board &board, int fromRow, int fromCol) const;
    std::unique_ptr<Piece> clone() const;
};

#endif
