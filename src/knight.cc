#include "knight.h"
#include <cmath>
using namespace std;

Knight::Knight(Colour colour): Piece(colour) {}

bool Knight::isLegalMove(const Board &board, int fromRow, int fromCol, int toRow, int toCol) const {
    int y = abs(toRow - fromRow);
    int x = abs(toCol - fromCol);
    return ((x == 2 && y == 1) || (y == 2 && x == 1));
}

char Knight::getSymbol() const {
    if (getColour() == Colour::White) {
        return 'N';
    } else if (getColour() == Colour::Black) {
        return 'n';
    } else if (getColour() == Colour::Red) {
        return 'G';
    } else {
        return 'g';
    }
}

PieceType Knight::getType() const {
    return PieceType::KNIGHT;
}

std::vector<Move> Knight::getLegalMoves(const Board &board, int fromRow, int fromCol) const {
    std::vector<Move> moves;
    Colour myColour = board.getCell(fromRow, fromCol).getPiece()->getColour();

    int jumps[8][2] = {
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
        {1, -2}, {1, 2}, {2, -1}, {2, 1}
    };

    for (auto& j : jumps) {
        int r = fromRow + j[0];
        int c = fromCol + j[1];
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

    return moves;
}

std::unique_ptr<Piece> Knight::clone() const {
  return std::make_unique<Knight>(getColour());
}
