#include "bishop.h"
#include <cmath>
using namespace std;

Bishop::Bishop(Colour colour): Piece(colour) {}

bool Bishop::isLegalMove(const Board &board, int fromRow, int fromCol, int toRow, int toCol) const {
    if (abs(toRow - fromRow) == abs(toCol - fromCol)) {
        if (board.isPathClear(fromRow, fromCol, toRow, toCol)) {
            return true;
        }
    }
    return false;
}

char Bishop::getSymbol() const {
    if (getColour() == Colour::White) {
        return 'B';
    } else if (getColour() == Colour::Black) {
        return 'b';
    } else if (getColour() == Colour::Red) {
        return 'F';
    } else {
        return 'f';
    }
}


Bishop::~Bishop() {}

PieceType Bishop::getType() const {
    return PieceType::BISHOP;
}

std::vector<Move> Bishop::getLegalMoves(const Board &board, int fromRow, int fromCol) const {
    std::vector<Move> legalMoves;
    Colour myColour = board.getCell(fromRow, fromCol).getPiece()->getColour();

    // 4 diagonal directions
    int directions[4][2] = {
        {-1, -1}, {-1, 1},
        {1, -1}, {1, 1}
    };

    for (auto& dir : directions) {
        int r = fromRow + dir[0];
        int c = fromCol + dir[1];

        while (r >= 0 && r < 8 && c >= 0 && c < 8) {
            Piece* target = board.getCell(r, c).getPiece();
            if (target == nullptr) {
                legalMoves.emplace_back(fromRow, fromCol, r, c);
            } else {
                if (target->getColour() != myColour) {
                    Move captureMove(fromRow, fromCol, r, c);
                    captureMove.isCapture = true;
                    captureMove.capturedPiece = target->getType();
                    legalMoves.push_back(captureMove);
                }
                break; 
            }
            r += dir[0];
            c += dir[1];
        }
    }

    return legalMoves;
}

std::unique_ptr<Piece> Bishop::clone() const {
  return std::make_unique<Bishop>(getColour());
}

