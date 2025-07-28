#ifndef QUEEN_H
#define QUEEN_H

#include "piece.h"
#include "board.h"

class Queen : public Piece {
public:
    // Constructor: initialize Queen with a specific colour
    explicit Queen(Colour colour);

    // Returns true if moving from (fromRow, fromCol) to (toRow, toCol) is a legal move for a Queen.
    bool isLegalMove(const Board &board, int fromRow, int fromCol, int toRow, int toCol) const override;

    // Returns the symbol representing the Queen.
    // 'Q' for white, 'q' for black.
    char getSymbol() const override;
    PieceType getType() const ;
    std::vector<Move> getLegalMoves(const Board &board, int fromRow, int fromCol) const;

    ~Queen() override = default; // default destructor

    std::unique_ptr<Piece> clone() const override;

};

#endif
