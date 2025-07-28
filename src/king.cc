#include "king.h"
#include <cmath>
using namespace std;

King::King(Colour colour): Piece(colour) {}

bool King::isLegalMove(const Board &board, int fromRow, int fromCol, int toRow, int toCol) const {
    int rowDiff = abs(toRow - fromRow);
    int colDiff = abs(toCol - fromCol);
    Colour colour = getColour();

    // regular move: one square in any direction
    if (rowDiff <= 1 && colDiff <= 1) {
        return true; // Board::move() ensures destination is valid and no check
    }

    // Castling: king moves two squares horizontally
    if (rowDiff != 0 || colDiff != 2) return false;

    // check for king and rook movement flags
    bool kingMoved, kingsideRookMoved, queensideRookMoved;
    if (colour == Colour::White) {
        kingMoved = board.whiteKingMoved;
        kingsideRookMoved = board.whiteKingsideRookMoved;
        queensideRookMoved = board.whiteQueensideRookMoved;
    } else if (colour == Colour::Black) {
        kingMoved = board.blackKingMoved;
        kingsideRookMoved = board.blackKingsideRookMoved;
        queensideRookMoved = board.blackQueensideRookMoved;
    } else if (colour == Colour::Red) {
        kingMoved = board.redKingMoved;
        kingsideRookMoved = board.redKingsideRookMoved;
        queensideRookMoved = board.redQueensideRookMoved;
    } else if (colour == Colour::Blue) {
        kingMoved = board.blueKingMoved;
        kingsideRookMoved = board.blueKingsideRookMoved;
        queensideRookMoved = board.blueQueensideRookMoved;
    } 
    else {
        return false; // invalid colour (i.e. Draw)
    }

    if (kingMoved) return false;

    // kingside castling (king to col 6)
    if (toCol == 6) {
        if (kingsideRookMoved) return false;
        if (!board.isPathClear(fromRow, fromCol, fromRow, 6)) {
            return false;
        }
        // check if king or castling squares are attacked by any opponent
        for (Colour opponent : {Colour::White, Colour::Black, Colour::Red, Colour::Blue}) {
            if (opponent != colour && (
                board.isInCheck(colour) ||
                board.squareUnderAttack(fromRow, 5, opponent) ||
                board.squareUnderAttack(fromRow, 6, opponent))) {
                return false;
            }
        }
        return true;
    }
    // Queenside castling (king to col 2)
    else if (toCol == 2) {
        if (queensideRookMoved) return false;
        if (!board.isPathClear(fromRow, fromCol, fromRow, 1)) return false;
        // Check if king or castling squares are attacked by any opponent
        for (Colour opponent : {Colour::White, Colour::Black, Colour::Red, Colour::Blue}) {
            if (opponent != colour && (
                board.isInCheck(colour) ||
                board.squareUnderAttack(fromRow, 3, opponent) ||
                board.squareUnderAttack(fromRow, 2, opponent))) {
                return false;
            }
        }
        return true;
    }

    return false;
}

char King::getSymbol() const {
    if (getColour() == Colour::White) {
        return 'K';
    } else if (getColour() == Colour::Black) {
        return 'k';
    } else if (getColour() == Colour::Red) {
        return 'A';
    } else {
        return 'a';
    }
}

PieceType King::getType() const {
    return PieceType::KING;
}

std::vector<Move> King::getLegalMoves(const Board &board, int fromRow, int fromCol) const {
    std::vector<Move> moves;
    Colour myColour = board.getCell(fromRow, fromCol).getPiece()->getColour();

    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            int r = fromRow + dr;
            int c = fromCol + dc;
            if (r < 0 || r >= 8 || c < 0 || c >= 8) continue;

            Piece* target = board.getCell(r, c).getPiece();
            if (!target) {
                moves.emplace_back(fromRow, fromCol, r, c);
            } else if (target->getColour() != myColour) {
                Move capture(fromRow, fromCol, r, c);
                capture.isCapture = true;
                capture.capturedPiece = target->getType();
                moves.push_back(capture);
            }
        }
    }
    return moves;
}

std::unique_ptr<Piece> King::clone() const {
  return std::make_unique<King>(getColour());
}
