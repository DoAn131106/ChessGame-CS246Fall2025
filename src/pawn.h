#ifndef PAWN_H
#define PAWN_H

#include "piece.h"
#include "board.h"

class Pawn : public Piece {
public:
    // Constructor: initialize Pawn with a specific colour
    explicit Pawn(Colour colour);

    // Returns true if moving from (fromRow, fromCol) to (toRow, toCol) is a legal move for a Pawn.
    bool isLegalMove(const Board &board, int fromRow, int fromCol, int toRow, int toCol) const override;

    // Returns the symbol representing the Pawn.
    // 'P' for white, 'p' for black.
    char getSymbol() const override;
    PieceType getType() const;
    std::vector<Move> getLegalMoves(const Board &board, int fromRow, int fromCol) const;
    ~Pawn() override = default; // default destructor

    std::unique_ptr<Piece> clone() const;
};

#endif
