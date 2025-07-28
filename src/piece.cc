#include "piece.h"
#include <stdexcept>

Move::Move(const std::string &from, const std::string &to, const std::string &promo)
    : isCapture(false), capturedPiece(PieceType::PAWN), promotion(promo), score(0) {
    if (from.length() < 2 || to.length() < 2) {
        throw std::invalid_argument("Invalid move format: from or to position too short");
    }
    fromCol = from[0] - 'a'; // a=0, b=1, ..., h=7
    fromRow = 8 - (from[1] - '0'); // 1=7, 2=6, ..., 8=0
    toCol = to[0] - 'a';
    toRow = 8 - (to[1] - '0');
    if (fromRow < 0 || fromRow >= 8 || fromCol < 0 || fromCol >= 8 ||
        toRow < 0 || toRow >= 8 || toCol < 0 || toCol >= 8) {
        throw std::invalid_argument("Invalid move: coordinates out of bounds");
    }
    if (!promo.empty()) {
        if (promo != "Q" && promo != "R" && promo != "B" && promo != "N") {
            throw std::invalid_argument("Invalid promotion piece");
        }
    }
}

std::string Move::toString() const {
    std::string from = std::string(1, 'a' + fromCol) + std::to_string(8 - fromRow);
    std::string to = std::string(1, 'a' + toCol) + std::to_string(8 - toRow);
    return from + to + (promotion.empty() ? "" : "=" + promotion);
}
