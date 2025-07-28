#ifndef KNIGHT_H
#define KNIGHT_H

#include "piece.h"
#include "board.h"

class Knight : public Piece {
public:
    // Constructor: initialize Knight with a specific colour
    explicit Knight(Colour colour);

    // Returns true if moving from (fromRow, fromCol) to (toRow, toCol) is a legal move for a Knight.
    bool isLegalMove(const Board &board, int fromRow, int fromCol, int toRow, int toCol) const override;

    // Returns the symbol representing the Knight.
    // 'N' for white, 'n' for black.
    char getSymbol() const override;
    PieceType getType() const;
    std::vector<Move> getLegalMoves(const Board &board, int fromRow, int fromCol) const;
    ~Knight() override = default; // default destructor

    std::unique_ptr<Piece> clone() const override;
};

#endif
