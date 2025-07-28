#include "rook.h"
#include <cmath>
using namespace std;

Rook::Rook(Colour colour): Piece(colour) {}

bool Rook::isLegalMove(const Board &board, int fromRow, int fromCol, int toRow, int toCol) const {
    if (!(fromRow == toRow || fromCol == toCol)) return false;
    if (!board.isPathClear(fromRow, fromCol, toRow, toCol)) return false;
    return true;
}

char Rook::getSymbol() const {
    if (colour == Colour::White) {
        return 'R';
    } else if (colour == Colour::Black) {
        return 'r';
    } else if (colour == Colour::Red) {
        return 'D';
    } else {
        return 'd';
    }
}

PieceType Rook::getType() const {
    return PieceType::ROOK;
}

std::vector<Move> Rook::getLegalMoves(const Board &board, int fromRow, int fromCol) const {
    std::vector<Move> moves;
    Colour myColour = board.getCell(fromRow, fromCol).getPiece()->getColour();

    int directions[4][2] = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    };

    for (auto& dir : directions) {
        int r = fromRow + dir[0];
        int c = fromCol + dir[1];
        while (r >= 0 && r < 8 && c >= 0 && c < 8) {
            Piece* target = board.getCell(r, c).getPiece();
            if (!target) {
                moves.emplace_back(fromRow, fromCol, r, c);
            } else {
                if (target->getColour() != myColour) {
                    Move capture(fromRow, fromCol, r, c);
                    capture.isCapture = true;
                    capture.capturedPiece = target->getType();
                    moves.push_back(capture);
                }
                break;
            }
            r += dir[0];
            c += dir[1];
        }
    }

    return moves;
}

std::unique_ptr<Piece> Rook::clone() const {
  return std::make_unique<Rook>(getColour());
}
