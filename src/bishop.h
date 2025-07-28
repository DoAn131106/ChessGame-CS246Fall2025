#ifndef BISHOP_H
#define BISHOP_H

#include "piece.h"
#include "board.h"

class Bishop : public Piece {
public:
    // Constructor: initialize Bishop with a specific colour
    explicit Bishop(Colour colour);

    // Returns true if moving from (fromRow, fromCol) to (toRow, toCol) is a legal move for a Bishop.
    bool isLegalMove(const Board &board, int fromRow, int fromCol, int toRow, int toCol) const override;

    // Returns the symbol representing the Bishop.
    // 'B' for white, 'b' for black.
    char getSymbol() const override;
    std::unique_ptr<Piece> clone() const;
    std::vector<Move> getLegalMoves(const Board &board, int fromRow, int fromCol) const;
    PieceType getType() const;

    ~Bishop() override; // default destructor
};

#endif
