#ifndef PIECE_H
#define PIECE_H
#include <string>
#include "board.h"

enum class Colour { White, Black, Red, Blue, Draw };
enum class PieceType { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };

struct Move {
  int fromRow, fromCol, toRow, toCol;
  bool isCapture;
  PieceType capturedPiece;
  std::string promotion;
  int score;

  Move()
      : fromRow(-1), fromCol(-1), toRow(-1), toCol(-1), isCapture(false),
        capturedPiece(PieceType::PAWN), score(0) {}
  Move(int fr, int fc, int tr, int tc, std::string promo = "")
      : fromRow(fr), fromCol(fc), toRow(tr), toCol(tc), isCapture(false),
        capturedPiece(PieceType::PAWN), promotion(promo), score(0) {}
  Move(const std::string &from, const std::string &to,
       const std::string &promo = "");
  std::string toString() const;
};

class Board; 

class Piece {
protected:
    Colour colour;
public:
    Piece(Colour c) : colour{c} {}
    virtual ~Piece() = default;
    
    virtual std::unique_ptr<Piece> clone() const = 0;

    Colour getColour() const { return colour; }

    // Checks if this piece can move from (fromRow, fromCol) to (toRow, toCol)
    virtual bool isLegalMove(const Board &board, int fromRow, int fromCol, int toRow, int toCol) const = 0;

    virtual PieceType getType() const = 0;
    virtual std::vector<Move> getLegalMoves(const Board &board, int fromRow, int fromCol) const = 0;

    // Returns display character: e.g., 'K' or 'k'
    virtual char getSymbol() const = 0;
};

#endif
