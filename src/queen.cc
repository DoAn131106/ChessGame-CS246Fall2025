#include "queen.h"
#include <cmath>
using namespace std;

Queen::Queen(Colour colour): Piece(colour) {}

bool Queen::isLegalMove(const Board &board, int fromRow, int fromCol, int toRow, int toCol) const {
    int rowDiff = abs(fromRow - toRow);
    int colDiff = abs(fromCol - toCol);

    bool validDirection = (rowDiff == colDiff) || (fromRow == toRow) || (fromCol == toCol);
    if (!validDirection) return false;

    if (!board.isPathClear(fromRow, fromCol, toRow, toCol)) return false;
    return true;
}

char Queen::getSymbol() const {
    if (colour == Colour::White) {
        return 'Q';
    } else if (colour == Colour::Black) {
        return 'q';
    } else if (colour == Colour::Red) {
        return 'S';
    } else {
        return 's';
    }
}

PieceType Queen::getType() const {
    return PieceType::QUEEN;
}

std::vector<Move> Queen::getLegalMoves(const Board &board, int fromRow, int fromCol) const {
    std::vector<Move> moves;

    // Use Rook and Bishop logic (8 directions)
    int directions[8][2] = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1},    // rook
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}   // bishop
    };

    Colour myColour = board.getCell(fromRow, fromCol).getPiece()->getColour();

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

std::unique_ptr<Piece> Queen::clone() const {
  return std::make_unique<Queen>(getColour());
}
