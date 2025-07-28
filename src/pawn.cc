#include "pawn.h"
#include <cmath>
using namespace std;

Pawn::Pawn(Colour colour): Piece(colour) {}

bool Pawn::isLegalMove(const Board &board, int fromRow, int fromCol, int toRow, int toCol) const {
    Colour colour = getColour();
    int rowDiff = toRow - fromRow;
    int colDiff = toCol - fromCol;
    bool isFourPlayer = board.fourplayermode();
    int size = isFourPlayer ? 14 : 8;

    const Cell &targetCell = board.getCell(toRow, toCol);
    bool hasTarget = targetCell.getPiece();

    switch (colour) {
        case Colour::White: {
            int startRow = size - 2; // 12
            if (colDiff == 0) {
                if (rowDiff == -1 && !hasTarget) return true;
                if (rowDiff == -2 && fromRow == startRow &&
                    !board.getCell(fromRow - 1, fromCol).getPiece() &&  
                    !hasTarget) return true;
            }
            if (rowDiff == -1 && abs(colDiff) == 1 && hasTarget &&
                targetCell.getPiece()->getColour() != colour)
                return true;
            break;
        }
        case Colour::Black: {
            int startRow = 1; 
            if (colDiff == 0) {
                if (rowDiff == 1 && !hasTarget) return true;
                if (rowDiff == 2 && fromRow == startRow &&
                    !board.getCell(fromRow + 1, fromCol).getPiece() &&  
                    !hasTarget) return true;
            }
            if (rowDiff == 1 && abs(colDiff) == 1 && hasTarget &&
                targetCell.getPiece()->getColour() != colour)
                return true;
            break;
        }
        case Colour::Red: {
            int startCol = 1;  
            if (rowDiff == 0) {
                if (colDiff == 1 && !hasTarget) return true;
                if (colDiff == 2 && fromCol == startCol &&
                    !board.getCell(fromRow, fromCol + 1).getPiece() &&  
                    !hasTarget) return true;
            }
            if (colDiff == 1 && abs(rowDiff) == 1 && hasTarget &&
                targetCell.getPiece()->getColour() != colour)
                return true;
            break;
        }
        case Colour::Blue: {
            int startCol = size - 2; // 12
            if (rowDiff == 0) {
                if (colDiff == -1 && !hasTarget) return true;
                if (colDiff == -2 && fromCol == startCol &&
                    !board.getCell(fromRow, fromCol - 1).getPiece() &&
                    !hasTarget) return true;
            }
            if (colDiff == -1 && abs(rowDiff) == 1 && hasTarget &&
                targetCell.getPiece()->getColour() != colour)
                return true;
            break;
        }
    }
    return false;
}

char Pawn::getSymbol() const {
    if (getColour() == Colour::White) {
        return 'P';
    } else if (getColour() == Colour::Black){
        return 'p';
    } else if (getColour() == Colour::Red) {
        return 'H';
    } else {
        return 'h';
    }
}


PieceType Pawn::getType() const {
    return PieceType::PAWN;
}

std::vector<Move> Pawn::getLegalMoves(const Board &board, int fromRow, int fromCol) const {
    std::vector<Move> moves;
    Colour myColour = board.getCell(fromRow, fromCol).getPiece()->getColour();
    int dir = (myColour == Colour::White) ? -1 : 1;

    int nextRow = fromRow + dir;

    // Forward move
    if (nextRow >= 0 && nextRow < 8 && !board.getCell(nextRow, fromCol).getPiece()) {
        moves.emplace_back(fromRow, fromCol, nextRow, fromCol);

        // First double move
        if ((myColour == Colour::White && fromRow == 6) || (myColour == Colour::Black && fromRow == 1)) {
            int doubleRow = fromRow + 2 * dir;
            if (!board.getCell(doubleRow, fromCol).getPiece()) {
                moves.emplace_back(fromRow, fromCol, doubleRow, fromCol);
            }
        }
    }

    // Captures
    for (int dc : {-1, 1}) {
        int captureCol = fromCol + dc;
        if (captureCol >= 0 && captureCol < 8) {
            Piece* target = board.getCell(nextRow, captureCol).getPiece();
            if (target && target->getColour() != myColour) {
                Move capture(fromRow, fromCol, nextRow, captureCol);
                capture.isCapture = true;
                capture.capturedPiece = target->getType();
                moves.push_back(capture);
            }
        }
    }

    return moves;
}


std::unique_ptr<Piece> Pawn::clone() const {
  return std::make_unique<Pawn>(getColour());
}
