#include "player.h"
#include "board.h"
#include "info.h"
#include "piece.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <climits>
#include <cmath>

// Player Base Class 
Player::Player(Colour c) : colour(c) {}

Colour Player::getColour() const {
    return colour;
}


// HumanPlayer 
HumanPlayer::HumanPlayer(Colour c) : Player(c) {}

// Human moves are handled by the CommandInterpreter
// This method should not be called directly for human players
// The CommandInterpreter will call board.move() directly
Move HumanPlayer::makeMove(Board& board, const std::string& from, const std::string& to, const std::string& promo) {
    try {
        Move move(from, to, promo);
        bool success = false;
        if (!promo.empty()) {
            char toChar = promo[0];
            success = board.promotion(move.fromRow, move.fromCol, move.toRow, move.toCol, toChar);
        } else {
            success = board.move(move.fromRow, move.fromCol, move.toRow, move.toCol);
        }
        return success ? move : Move();
    } catch (const std::invalid_argument& e) {
        return Move(); // Invalid move format
    }
}

std::string HumanPlayer::getPlayerType() const {
    return "human";
}

// ComputerPlayer Base Class 
ComputerPlayer::ComputerPlayer(Colour c, int diff) : Player(c), difficulty(diff) {}

int ComputerPlayer::getPieceValue(PieceType piece) const {
    switch (piece) {
        case PieceType::PAWN: return 100;
        case PieceType::KNIGHT: return 300;
        case PieceType::BISHOP: return 300;
        case PieceType::ROOK: return 500;
        case PieceType::QUEEN: return 900;
        case PieceType::KING: return 10000;
        default: return 0;
    }
}

Move ComputerPlayer::makeMove(Board& board, const std::string& from, const std::string& to, const std::string& promo) {
    Move bestMove = selectMove(board);
    if (bestMove.fromRow == -1) {
        return Move(); // No legal moves
    }
    bool success = !bestMove.promotion.empty() ?
        board.promotion(bestMove.fromRow, bestMove.fromCol, bestMove.toRow, bestMove.toCol, bestMove.promotion[0]) :
        board.move(bestMove.fromRow, bestMove.fromCol, bestMove.toRow, bestMove.toCol);
    return success ? bestMove : Move();
}

std::string ComputerPlayer::getPlayerType() const {
    return "computer[" + std::to_string(difficulty) + "]";
}

int ComputerPlayer::getDifficulty() const {
    return difficulty;
}

// Require virtual std::vector<std::pair<int, int>> getLegalMoves(const Board& board, int fromRow, int fromCol) const = 0; in Piece 
//          and implement each get legal move for each PieceType (Pawn, Rook, King, etc)
void ComputerPlayer::addMoveWithPromotions(std::vector<Move>& legalMoves, const Board& board,
                                          int fromRow, int fromCol, int toRow, int toCol) const {
    const Cell& fromCell = board.getCell(fromRow, fromCol);
    const Cell& toCell = board.getCell(toRow, toCol);
    Board tempBoard = board;
    if (tempBoard.move(fromRow, fromCol, toRow, toCol) && !tempBoard.isInCheck(colour)) {
        if (fromCell.getPiece()->getType() == PieceType::PAWN &&
            ((colour == Colour::White && toRow == 0) || (colour == Colour::Black && toRow == 7))) {
            for (const char* promo : {"Q", "R", "B", "N"}) {
                Move move(fromRow, fromCol, toRow, toCol, std::string(promo));
                if (toCell.getPiece() && toCell.getPiece()->getColour() != colour) {
                    move.isCapture = true;
                    move.capturedPiece = toCell.getPiece()->getType();
                    move.score = getPieceValue(move.capturedPiece) + 1000;
                } else {
                    move.score = 900; // Promotion bonus
                }
                legalMoves.push_back(move);
            }
        } else {
            Move move(fromRow, fromCol, toRow, toCol);
            if (toCell.getPiece() && toCell.getPiece()->getColour() != colour) {
                move.isCapture = true;
                move.capturedPiece = toCell.getPiece()->getType();
                move.score = getPieceValue(move.capturedPiece) + 1000;
            } else if (board.isEnPassantTarget(toRow, toCol) &&
                       ((colour == Colour::White && fromRow == 3 && toRow == 2 && abs(toCol - fromCol) == 1) ||
                        (colour == Colour::Black && fromRow == 4 && toRow == 5 && abs(toCol - fromCol) == 1))) {
                move.isCapture = true;
                move.capturedPiece = PieceType::PAWN;
                move.score = getPieceValue(PieceType::PAWN) + 1000;
            }
            legalMoves.push_back(move);
        }
    }
}

std::vector<Move> ComputerPlayer::getAllLegalMoves(const Board& board) const {
    std::vector<Move> legalMoves;
    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            const Cell& fromCell = board.getCell(fromRow, fromCol);
            if (!fromCell.getPiece() || fromCell.getPiece()->getColour() != colour) {
                continue;
            }
            auto candidateMoves = fromCell.getPiece()->getLegalMoves(board, fromRow, fromCol);
            for (const auto& move : candidateMoves) {
                if (!fromCell.getPiece()->isLegalMove(board, move.fromRow, move.fromCol, move.toRow, move.toCol)) {
                    continue;
                }
                addMoveWithPromotions(legalMoves, board, move.fromRow, move.fromCol, move.toRow, move.toCol);
            }
        }
    }
    std::sort(legalMoves.begin(), legalMoves.end(), [this](const Move& a, const Move& b) {
        return a.score > b.score;
    });
    return legalMoves;
}

// Level 1 
Level1Computer::Level1Computer(Colour c) : ComputerPlayer(c, 1) {}

Move Level1Computer::makeMove(Board& board, const std::string& from, const std::string& to, const std::string& promo) {
    return ComputerPlayer::makeMove(board, from, to, promo);
}


int Level1Computer::evaluateBoard(const Board& board) const {
    // Level 1 doesn't really evaluate - just random
    return 0;
}

Move Level1Computer::selectMove(const Board& board) const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::vector<Move> legalMoves = getAllLegalMoves(board);
    if (legalMoves.empty()) {
        return Move();
    }
    std::uniform_int_distribution<> dis(0, legalMoves.size() - 1);
    return legalMoves[dis(gen)];
}

// Level 2 
Level2Computer::Level2Computer(Colour c) : ComputerPlayer(c, 2) {}

Move Level2Computer::makeMove(Board& board, const std::string& from, const std::string& to, const std::string& promo) {
    return ComputerPlayer::makeMove(board, from, to, promo);
}


int Level2Computer::evaluateBoard(const Board& board) const {
    int score = 0;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const Cell& cell = board.getCell(row, col); // Use const version (make sure to have a const version in Board)
            if (cell.getPiece()) {
                int pieceValue = getPieceValue(cell.getPiece()->getType());
                if (cell.getPiece()->getColour() == colour) {
                    score += pieceValue;
                } else {
                    score -= pieceValue;
                }
            }
        }
    }
    return score;
}

Move Level2Computer::selectMove(const Board& board) const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::vector<Move> legalMoves = getAllLegalMoves(board);

    if (legalMoves.empty()) {
        return Move();
    }
    
    // First, try to find capturing moves
    std::vector<Move> capturingMoves = getCapturesMoves(legalMoves, board);
    if (!capturingMoves.empty()) {
        // Return the capture with highest value
        std::sort(capturingMoves.begin(), capturingMoves.end(),
                  [this](const Move& a, const Move& b) {
                      return getPieceValue(a.capturedPiece) > getPieceValue(b.capturedPiece);
                  });
        return capturingMoves[0];
    }
    
    // Then, try to find checking moves
    std::vector<Move> checkingMoves = getCheckMoves(legalMoves, board);
    if (!checkingMoves.empty()) {
        std::uniform_int_distribution<> dis(0, checkingMoves.size() - 1);
        return checkingMoves[dis(gen)];
    }

    // Otherwise, random legal move
    std::uniform_int_distribution<> dis(0, legalMoves.size() - 1);
    return legalMoves[dis(gen)];
    
}

std::vector<Move> Level2Computer::getCapturesMoves(const std::vector<Move>& moves, const Board& board) const {
    std::vector<Move> capturingMoves;
    
    for (const auto& move : moves) {
        if (move.isCapture) {
            capturingMoves.push_back(move);
        }
    }
    
    return capturingMoves;
}

std::vector<Move> Level2Computer::getCheckMoves(const std::vector<Move>& moves, const Board& board) const {
    std::vector<Move> checkingMoves;
    
    for (const auto& move : moves) {
        Board tempBoard = board;
        if (tempBoard.move(move.fromRow, move.fromCol, move.toRow, move.toCol)) {
            Colour opponent = (colour == Colour::White) ? Colour::Black : Colour::White;
            if (tempBoard.isInCheck(opponent)) {
                checkingMoves.push_back(move);
            }
        }
    }
    
    return checkingMoves;
}

// Level 3 
Level3Computer::Level3Computer(Colour c) : ComputerPlayer(c, 3) {}

Move Level3Computer::makeMove(Board& board, const std::string& from, const std::string& to, const std::string& promo) {
    return ComputerPlayer::makeMove(board, from, to, promo);
}


int Level3Computer::evaluateBoard(const Board& board) const {
    int materialScore = 0;

    // Count material
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const Cell& cell = board.getCell(row, col); // Use const version
            if (cell.getPiece()) {
                int pieceValue = getPieceValue(cell.getPiece()->getType());
                if (cell.getPiece()->getColour() == colour) {
                    materialScore += pieceValue;
                } else {
                    materialScore -= pieceValue;
                }
            }
        }
    }
    int positionalScore = 0;
    if (board.isInCheck(colour)) {
        positionalScore -= 50;
    }
    Colour opponent = (colour == Colour::White) ? Colour::Black : Colour::White;
    if (board.isInCheck(opponent)) {
        positionalScore += 50;
    }
    return materialScore + positionalScore;
}

Move Level3Computer::selectMove(const Board& board) const {
    std::vector<Move> legalMoves = getAllLegalMoves(board);
    
    if (legalMoves.empty()) {
        return Move();
    }
    
    // Filter out unsafe moves
    std::vector<Move> safeMoves = getSafeMoves(legalMoves, board);
    std::vector<Move> movesToConsider = safeMoves.empty() ? legalMoves : safeMoves;
    
    // Score all moves
    Move bestMove;
    int bestScore = INT_MIN;
    
    for (auto& move : movesToConsider) {
        int score = evaluateMoveScore(board, move);
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }
    
    return bestMove;
}

std::vector<Move> Level3Computer::getSafeMoves(const std::vector<Move>& moves, const Board& board) const {
    std::vector<Move> safeMoves;
    
    for (const auto& move : moves) {
        if (isMoveSafe(board, move)) {
            safeMoves.push_back(move);
        }
    }
    
    return safeMoves;
}

int Level3Computer::evaluateMoveScore(const Board& board, const Move& move) const {
    int score = 0;
    
    // Bonus for captures
    if (move.isCapture) {
        score += 10 + getPieceValue(move.capturedPiece);
    }
    
    // Bonus for checks
    Board tempBoard = board;
    if (tempBoard.move(move.fromRow, move.fromCol, move.toRow, move.toCol)) {
        Colour opponent = (colour == Colour::White) ? Colour::Black : Colour::White;
        if (tempBoard.isInCheck(opponent)) {
            score += 20;
        }
    }
    
    // Positional bonus for center control
    if ((move.toRow == 3 || move.toRow == 4) && (move.toCol == 3 || move.toCol == 4)) {
        score += 5;
    }
    
    return score;
}

bool Level3Computer::isMoveSafe(const Board& board, const Move& move) const {
    Board tempBoard = board;
    
    // Make the move
    if (!tempBoard.move(move.fromRow, move.fromCol, move.toRow, move.toCol)) {
        return false;
    }
    
    // Check if we're in check after the move
    if (tempBoard.isInCheck(colour)) {
        return false;
    }
    
    return true;
}

// Level 4
Level4Computer::Level4Computer(Colour c) : ComputerPlayer(c, 4) {}

Move Level4Computer::makeMove(Board& board, const std::string& from, const std::string& to, const std::string& promo) {
    return ComputerPlayer::makeMove(board, from, to, promo);
}

int Level4Computer::evaluateBoard(const Board& board) const {
    return evaluatePosition(board);
}

Move Level4Computer::selectMove(const Board& board) const {
    std::vector<Move> legalMoves = getAllLegalMoves(board);
    if (legalMoves.empty()) {
        return Move();
    }
    Move bestMove;
    int bestScore = INT_MIN;
    for (const auto& move : legalMoves) {
        Board tempBoard = board;
        bool success = !move.promotion.empty() ?
            tempBoard.promotion(move.fromRow, move.fromCol, move.toRow, move.toCol, move.promotion[0]) :
            tempBoard.move(move.fromRow, move.fromCol, move.toRow, move.toCol);
        if (success) {
            // Score move for sorting purposes
            int moveScore = move.isCapture ? getPieceValue(move.capturedPiece) + 1000 : 0;
            if (!move.promotion.empty()) moveScore += 900;
            if (tempBoard.isInCheck(colour == Colour::White ? Colour::Black : Colour::White)) {
                moveScore += 500;
            }
            int score = minimax(tempBoard, MAX_DEPTH - 1, false, INT_MIN, INT_MAX);
            // Adjust score by move priority to break ties
            score += moveScore / 100; // Scale down to prioritize minimax score
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
        }
    }
    return bestMove;
}

int Level4Computer::minimax(Board board, int depth, bool isMaximizing, int alpha, int beta) const {
    if (depth == 0) {
        return evaluatePosition(board);
    }
    Colour currentColor = isMaximizing ? colour : (colour == Colour::White ? Colour::Black : Colour::White);
    if (board.isCheckmate(currentColor)) {
        return isMaximizing ? INT_MIN + depth : INT_MAX - depth;
    }
    if (!board.hasAnyLegalMove(currentColor)) {
        return 0; // Stalemate
    }
    std::vector<Move> moves = getAllLegalMoves(board); // Use inherited method
    // Sort moves for better pruning
    std::sort(moves.begin(), moves.end(), [this, &board](const Move& a, const Move& b) {
        int scoreA = a.isCapture ? getPieceValue(a.capturedPiece) + 1000 : 0;
        int scoreB = b.isCapture ? getPieceValue(b.capturedPiece) + 1000 : 0;
        if (!a.promotion.empty()) scoreA += 900;
        if (!b.promotion.empty()) scoreB += 900;
        return scoreA > scoreB;
    });
    if (isMaximizing) {
        int maxScore = INT_MIN;
        for (const auto& move : moves) {
            Board tempBoard = board;
            bool success = !move.promotion.empty() ?
                tempBoard.promotion(move.fromRow, move.fromCol, move.toRow, move.toCol, move.promotion[0]) :
                tempBoard.move(move.fromRow, move.fromCol, move.toRow, move.toCol);
            if (success) {
                int score = minimax(tempBoard, depth - 1, false, alpha, beta);
                maxScore = std::max(maxScore, score);
                alpha = std::max(alpha, score);
                if (beta <= alpha) {
                    break; // Alpha-beta pruning
                }
            }
        }
        return maxScore;
    } else {
        int minScore = INT_MAX;
        for (const auto& move : moves) {
            Board tempBoard = board;
            bool success = !move.promotion.empty() ?
                tempBoard.promotion(move.fromRow, move.fromCol, move.toRow, move.toCol, move.promotion[0]) :
                tempBoard.move(move.fromRow, move.fromCol, move.toRow, move.toCol);
            if (success) {
                int score = minimax(tempBoard, depth - 1, true, alpha, beta);
                minScore = std::min(minScore, score);
                beta = std::min(beta, score);
                if (beta <= alpha) {
                    break; // Alpha-beta pruning
                }
            }
        }
        return minScore;
    }
}

int Level4Computer::evaluatePosition(const Board& board) const {
    int materialScore = 0;
    
    // Material evaluation
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const Cell& cell = board.getCell(row, col); // Use const version
            if (cell.getPiece()) {
                int pieceValue = getPieceValue(cell.getPiece()->getType());
                if (cell.getPiece()->getColour() == colour) {
                    materialScore += pieceValue;
                } else {
                    materialScore -= pieceValue;
                }
            }
        }
    }
    
    // King safety
    int kingSafety = 0;
    if (board.isInCheck(colour)) {
        kingSafety -= 50;
    }
    Colour opponent = (colour == Colour::White) ? Colour::Black : Colour::White;
    if (board.isInCheck(opponent)) {
        kingSafety += 50;
    }
    
    // Mobility (simplified - count legal moves)
    int mobility = countLegalMoves(board, colour) - countLegalMoves(board, opponent);
    
    return materialScore + kingSafety + mobility;
}


int Level4Computer::countLegalMoves(const Board& board, Colour color) const {
    Level1Computer tempPlayer(color);
    return tempPlayer.getAllLegalMoves(board).size();
}
