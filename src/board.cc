#include "board.h"
#include "textdisplay.h"
#include "graphicsdisplay.h"
#include "cell.h"
#include "bishop.h"
#include "king.h"
#include "knight.h"
#include "pawn.h"
#include "queen.h"
#include "rook.h"
#include <iostream>
#include <memory>
#include "piece.h"
#include <cmath>
#include <array>
#include <algorithm>
using namespace std;

Board::Board() {}

Board::~Board() {}

bool Board::isEnPassantTarget(int row, int col) const {
    return enPassant.valid && enPassant.row == row && enPassant.col == col;
}

void Board::clear() {
    for (int row = 0; row < board.size(); ++row) {
        for (int col = 0; col < board[row].size(); ++col) {
            board[row][col].setPiece(nullptr); // Remove piece from cell
        }
    }
}

bool Board::fourplayermode() const {
    if (board.size() == 14) return true;
    return false;
}

void Board::init(bool fourPlayerMode) {
    int n;
    if (fourPlayerMode) n = 14;
    else { n = 8; };
    td.reset();
    gd.reset();
    td = std::make_shared<TextDisplay>(fourPlayerMode);
    gd = std::make_shared<GraphicsDisplay>(fourPlayerMode);
    board.clear();
    board.resize(n);

    for (auto r = 0; r < static_cast<int>(n); r++) {
        board[r].reserve(n);
        for (auto c = 0; c < static_cast<int>(n); c++) {
            board[r].emplace_back(r, c);
            board[r][c].attach(td.get());
            board[r][c].attach(gd.get());
        }
    }
}

bool Board::isPathClear(int fromRow, int fromCol, int toRow, int toCol) const {
    int y = (toRow - fromRow);
    int x = (toCol - fromCol);
    if (y < 0) {
        y = -1;
    } else if (y > 0) {
        y = 1;
    }
    if (x < 0) {
        x = -1;
    } else if (x > 0) {
        x = 1;
    }
    int currRow = fromRow + y;
    int currCol = fromCol + x;

    while (currRow != toRow || currCol != toCol) {
        if (board[currRow][currCol].getPiece() != nullptr) {
            return false;
        }
        currRow += y;
        currCol += x;
    }
    return true;
}

bool Board::move(int fromRow, int fromCol, int toRow, int toCol) {
    if (fromRow < 0 || fromRow >= 8 || fromCol < 0 || fromCol >= 8 ||
        toRow < 0 || toRow >= 8 || toCol < 0 || toCol >= 8) {
        return false;
    }

    Cell &fromCell = board[fromRow][fromCol];
    Cell &toCell = board[toRow][toCol];

    Piece *piece = fromCell.getPiece();
    if (!piece) {
        return false;
    }
    Piece *pieceto = toCell.getPiece();
    std::unique_ptr<Piece> &fromPtr = fromCell.getPiecePtr();
    std::unique_ptr<Piece> &toPtr = toCell.getPiecePtr();
    std::swap(fromPtr, toPtr);
    bool inCheck = isInCheck(piece->getColour());
    std::swap(fromPtr, toPtr);
    if (inCheck) { 
        return false;
    }
    if (!piece->isLegalMove(*this, fromRow, fromCol, toRow, toCol)) {
        return false;
    }

    if (piece->getSymbol() == 'R' || piece->getSymbol() == 'r') {
        if (fromRow == 0 && fromCol == 0) whiteQueensideRookMoved = true;
        else if (fromRow == 0 && fromCol == 7) whiteKingsideRookMoved = true;
        else if (fromRow == 7 && fromCol == 0) blackQueensideRookMoved = true;
        else if (fromRow == 7 && fromCol == 7) blackKingsideRookMoved = true;
    }

    if (piece->getSymbol() == 'K' || piece->getSymbol() == 'k') {
        if (piece->getColour() == Colour::White) whiteKingMoved = true;
        else blackKingMoved = true;
    }

    if (auto *destPiece = toCell.getPiece()) {
        if (destPiece->getColour() == piece->getColour()) {
            return false;
        }
    }

    if (piece->getType() == PieceType::PAWN) { 
        if (!pawnlegal(fromRow, fromCol, toRow, toCol, piece->getColour())) {
            return false;
        } 
        if (abs(toCol - fromCol) == 1 && board[toRow][toCol].getPiece() == nullptr) {
            if (piece->getColour() == Colour::White && toRow == fromRow - 1) {
                if (enPassant.valid && enPassant.row == toRow && enPassant.col == toCol) {
                    board[fromRow][enPassant.col].removePiece();
                } 
                else {
                    return false; 
                }
            } 
            else if (piece->getColour() == Colour::Black && toRow == fromRow + 1) {
                if (enPassant.valid && enPassant.row == toRow && enPassant.col == toCol) {
                    board[fromRow][enPassant.col].removePiece();
                } 
                else {
                    return false; 
                }
            }
        } 
        else {
            enPassant.valid = false;
        }
    } 
    else {
        enPassant.valid = false;
    }

    auto movedPiece = fromCell.removePiece();
    toCell.setPiece(std::move(movedPiece));
    if (piece->getType() == PieceType::KING) {
        // Kingside castling
        if (fromCol == 4 && toCol == 6) {
            // White or Black kingside
            int row = (piece->getColour() == Colour::White) ? 7 : 0;
            auto rook = board[row][7].removePiece();  // rook from h1 or h8
            board[row][5].setPiece(std::move(rook));  // move to f1 or f8
        }
        // Queenside castling
        else if (fromCol == 4 && toCol == 2) {
            int row = (piece->getColour() == Colour::White) ? 7 : 0;
            auto rook = board[row][0].removePiece();  // rook from a1 or a8
            board[row][3].setPiece(std::move(rook));  // move to d1 or d8
        }
    }
    if (piece->getType() == PieceType::PAWN) {
        int direction = (piece->getColour() == Colour::White) ? -1 : 1;
        if (toRow - fromRow == 2 * direction) {
            enPassant.valid = true;
            enPassant.row = fromRow + direction;
            enPassant.col = fromCol;
        }
    }
    return true;
}

bool Board::move4(int fromRow, int fromCol, int toRow, int toCol, const std::vector<Colour>& lostPlayers) {
    if (!inbounds(fromRow, fromCol) || !inbounds(toRow, toCol)) {
        return false;
    }

    Cell &fromCell = board[fromRow][fromCol];
    Cell &toCell = board[toRow][toCol];

    Piece *piece = fromCell.getPiece();
    if (!piece) return false;
    Piece *pieceto = toCell.getPiece();
    std::unique_ptr<Piece> &fromPtr = fromCell.getPiecePtr();
    std::unique_ptr<Piece> &toPtr = toCell.getPiecePtr();
    std::swap(fromPtr, toPtr); 
    bool inCheck = isInCheck4(piece->getColour(), lostPlayers);
    std::swap(fromPtr, toPtr);
    if (inCheck) return false;
    if (!piece->isLegalMove(*this, fromRow, fromCol, toRow, toCol)) {
        return false;
    }
    Colour c = piece->getColour();
    if (piece->getType() == PieceType::ROOK) {
        if (c == Colour::White) {
            if (fromRow == 13 && fromCol == 3) whiteQueensideRookMoved = true;
            else if (fromRow == 13 && fromCol == 10) whiteKingsideRookMoved = true;
        } else if (c == Colour::Black) {
            if (fromRow == 0 && fromCol == 3) blackQueensideRookMoved = true;
            else if (fromRow == 0 && fromCol == 10) blackKingsideRookMoved = true;
        } else if (c == Colour::Red) {
            if (fromRow == 3 && fromCol == 0) redQueensideRookMoved = true;
            else if (fromRow == 10 && fromCol == 0) redKingsideRookMoved = true;
        } else if (c == Colour::Blue) {
            if (fromRow == 3 && fromCol == 13) blueQueensideRookMoved = true;
            else if (fromRow == 10 && fromCol == 13) blueKingsideRookMoved = true;
        }
    }

    if (piece->getType() == PieceType::KING) {
        if (c == Colour::White) whiteKingMoved = true;
        else if (c == Colour::Black) blackKingMoved = true;
        else if (c == Colour::Red) redKingMoved = true;
        else if (c == Colour::Blue) blueKingMoved = true;
    }

    if (auto *destPiece = toCell.getPiece()) {
        if (destPiece->getColour() == c) {
            return false;
        }
    }

    if (piece->getType() == PieceType::PAWN) { 
        if (!pawnlegal4(fromRow, fromCol, toRow, toCol, c)) {
            return false;
        } 
    }
    auto movedPiece = fromCell.removePiece();
    toCell.setPiece(std::move(movedPiece));
    if (piece->getType() == PieceType::KING) {
        // WHITE 
        if (c == Colour::White && fromRow == 13 && fromCol == 7) {
            whiteKingMoved = true;
            if (toCol == 10) {  
                auto rook = board[13][10].removePiece();  
                board[13][8].setPiece(std::move(rook)); 
                whiteKingsideRookMoved = true;
            } else if (toCol == 3) {  // Queenside castling
                auto rook = board[13][3].removePiece();  
                board[13][6].setPiece(std::move(rook));
                whiteQueensideRookMoved = true;
            }
        } else if (c == Colour::Black && fromRow == 0 && fromCol == 7) {
            blackKingMoved = true;
            if (toCol == 10) {  
                auto rook = board[0][10].removePiece();  
                board[0][8].setPiece(std::move(rook));  
                blackQueensideRookMoved = true;
            } else if (toCol == 3) {  // Queenside castling
                auto rook = board[0][3].removePiece();  
                board[0][6].setPiece(std::move(rook));  
                blackKingsideRookMoved = true;
            }
        } else if (c == Colour::Red && fromRow == 7 && fromCol == 0) {
            redKingMoved = true;
            if (toRow == 10) {  
                auto rook = board[10][0].removePiece();  
                board[8][0].setPiece(std::move(rook));  
                redKingsideRookMoved = true;
            } else if (toRow == 3) {  // Queenside castling
                auto rook = board[3][0].removePiece();  
                board[6][0].setPiece(std::move(rook)); 
                redQueensideRookMoved = true; 
            }
        } else if (c == Colour::Blue && fromRow == 7 && fromCol == 13) {
            blueKingMoved = true;
            if (toRow == 10) {  
                auto rook = board[10][13].removePiece();  
                board[8][13].setPiece(std::move(rook));  
                blueQueensideRookMoved = true;
            } else if (toRow == 3) {  // Queenside castling
                auto rook = board[3][13].removePiece();  
                board[6][13].setPiece(std::move(rook));  
                blueKingsideRookMoved = true;
            }
        }
    }
    return true;
}


bool Board::squareUnderAttack(int row, int col, Colour colour) const {
    for (int r = 0; r < board.size(); r++) {
        for (int c = 0; c < board[r].size(); c++) {
            Piece *attacker = board[r][c].getPiece();
            if (attacker && attacker->getColour() == colour) {
                if (attacker->isLegalMove(*this, r, c, row, col)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Board::isInCheck(Colour colour) const {
    int kingRow, kingCol;
    bool breakloop = false;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Piece *piece = board[r][c].getPiece();
            if (piece && piece->getColour() == colour && tolower(piece->getSymbol()) == 'k') {
                kingRow = r;
                kingCol = c;
                breakloop = true;
                break;
            }
        }
        if (breakloop) break;
    }   
    Colour enemy;
    if (colour == Colour::White) {
        enemy = Colour::Black;
    } else {
        enemy = Colour::White;
    }
    return squareUnderAttack(kingRow, kingCol, enemy);
}

bool Board::isInCheck4(Colour colour, const std::vector<Colour>& lostPlayers) const {
    int kingRow, kingCol;
    bool breakloop = false;
    for (int r = 0; r < board.size(); r++) {
        for (int c = 0; c < board[r].size(); c++) {
            Piece *piece = board[r][c].getPiece();
            if (piece && piece->getColour() == colour && piece->getType() == PieceType::KING) {
                kingRow = r;
                kingCol = c;
                breakloop = true;
                break;
            }
        }
        if (breakloop) break;
    }   
    std::array<Colour, 3> enemy;
    if (colour == Colour::White) {
        enemy = {Colour::Black, Colour::Red, Colour::Blue};
    } else if (colour == Colour::Black) {
        enemy = {Colour::White, Colour::Red, Colour::Blue};
    } else if (colour == Colour::Red) {
        enemy = {Colour::White, Colour::Black, Colour::Blue};
    } else {
        enemy = {Colour::White, Colour::Black, Colour::Red};
    }
    for (Colour c : enemy) {
        if (std::find(lostPlayers.begin(), lostPlayers.end(), c) != lostPlayers.end()) continue;
        if (squareUnderAttack(kingRow, kingCol, c)) return true;
    }
    return false;
}

bool Board::hasAnyLegalMove(Colour colour) {
    const int n = board.size();
    for (int fromRow = 0; fromRow < n; fromRow++) {
        for (int fromCol = 0; fromCol < n; fromCol++) {
            Cell &from = board[fromRow][fromCol];
            Piece *piece = board[fromRow][fromCol].getPiece();
            if (piece && piece->getColour() == colour) {
                for (int toRow = 0; toRow < n; toRow++) {
                    for (int toCol = 0; toCol < n; toCol++) {
                        if (piece->isLegalMove(*this, fromRow, fromCol, toRow, toCol)) {
                            Cell &to = board[toRow][toCol];
                            Piece *pieceto = board[toRow][toCol].getPiece();
                            std::unique_ptr<Piece> &fromPtr = from.getPiecePtr();
                            std::unique_ptr<Piece> &toPtr = to.getPiecePtr();
                            std::swap(fromPtr, toPtr);
                            bool inCheck = isInCheck(colour);
                            std::swap(fromPtr, toPtr);
                            if (!inCheck) return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool Board::hasAnyLegalMove4(Colour colour, const std::vector<Colour>& lostPlayers) {
    const int n = board.size();
    for (int fromRow = 0; fromRow < n; fromRow++) {
        for (int fromCol = 0; fromCol < n; fromCol++) {
            if (!inbounds(fromRow, fromCol)) continue;
            Cell &from = board[fromRow][fromCol];
            Piece *piece = board[fromRow][fromCol].getPiece();
            if (piece && piece->getColour() == colour) {
                for (int toRow = 0; toRow < n; toRow++) {
                    for (int toCol = 0; toCol < n; toCol++) {
                        if (!inbounds(toRow, toCol)) continue;
                        if (piece->isLegalMove(*this, fromRow, fromCol, toRow, toCol)) {
                            Cell &to = board[toRow][toCol];
                            Piece *pieceto = board[toRow][toCol].getPiece();
                            std::unique_ptr<Piece> &fromPtr = from.getPiecePtr();
                            std::unique_ptr<Piece> &toPtr = to.getPiecePtr();
                            std::swap(fromPtr, toPtr);
                            bool inCheck = isInCheck4(colour, lostPlayers);
                            std::swap(fromPtr, toPtr);
                            if (!inCheck) return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool Board::isCheckmate(Colour colour) const {
    return isInCheck(colour) && !const_cast<Board*>(this)->hasAnyLegalMove(colour);
}

bool Board::pawnlegal(int fromRow, int fromCol, int toRow, int toCol, Colour colour) {
    int rowDiff = toRow - fromRow;
    int colDiff = toCol - fromCol;
    Cell &toCell = board[toRow][toCol];
    int direction = (colour == Colour::White) ? -1 : 1; // White moves up, Black moves down
    int startRow = (colour == Colour::White) ? 6 : 1;

    // Forward move (one square)
    if (colDiff == 0 && rowDiff == direction) {
        return toCell.getPiece() == nullptr;
    }
    // Forward move (two squares from starting position)
    if (colDiff == 0 && rowDiff == 2 * direction && fromRow == startRow) {
        int midRow = fromRow + direction;
        if (board[midRow][fromCol].getPiece() == nullptr && toCell.getPiece() == nullptr) {
            enPassant.valid = true;
            enPassant.row = (fromRow + toRow) / 2;
            enPassant.col = fromCol;
            return true;
        }
        return false;
    }
    // Diagonal capture
    if (abs(colDiff) == 1 && rowDiff == direction) {
        if (toCell.getPiece() != nullptr && toCell.getPiece()->getColour() != colour) {
            return true;
        } else if (toCell.getPiece() == nullptr && enPassant.valid) {
            return true;
        }
    }
    return false;
}

std::pair<int, int> Board::getPawnDirection(Colour c) {
    if (c == Colour::White) return {-1, 0};
    if (c == Colour::Black) return {1, 0};
    if (c == Colour::Red)   return {0, 1};  // moves right
    if (c == Colour::Blue)  return {0, -1}; // moves left
    return {0, 0}; // invalid
}


bool Board::pawnlegal4(int fr, int fc, int tr, int tc, Colour colour) {
    int rowDiff = tr - fr;
    int colDiff = tc - fc;
    Cell &toCell = board[tr][tc];
    bool targetExists = toCell.getPiece();
    
    int size = 14;
    int startRow = -1, startCol = -1;

    switch (colour) {
        case Colour::White: {
            startRow = size - 2; // 12
            // Forward one
            if (rowDiff == -1 && colDiff == 0 && !targetExists) return true;
            // Double step
            if (rowDiff == -2 && colDiff == 0 && fr == startRow &&
                !board[fr - 1][fc].getPiece() && !targetExists)
                return true;
            // Capture
            if (rowDiff == -1 && abs(colDiff) == 1 && targetExists &&
                toCell.getPiece()->getColour() != colour)
                return true;
            break;
        }
        case Colour::Black: {
            startRow = 1;
            if (rowDiff == 1 && colDiff == 0 && !targetExists) return true;
            if (rowDiff == 2 && colDiff == 0 && fr == startRow &&
                !board[fr + 1][fc].getPiece() && !targetExists)
                return true;
            if (rowDiff == 1 && abs(colDiff) == 1 && targetExists &&
                toCell.getPiece()->getColour() != colour)
                return true;
            break;
        }
        case Colour::Red: {
            startCol = 1;
            if (colDiff == 1 && rowDiff == 0 && !targetExists) return true;
            if (colDiff == 2 && rowDiff == 0 && fc == startCol &&
                !board[fr][fc - 1].getPiece() && !targetExists)
                return true;
            if (colDiff == 1 && abs(rowDiff) == 1 && targetExists &&
                toCell.getPiece()->getColour() != colour)
                return true;
            break;
        }
        case Colour::Blue: {
            startCol = size - 2; // 12
            if (colDiff == -1 && rowDiff == 0 && !targetExists) return true;
            if (colDiff == -2 && rowDiff == 0 && fc == startCol &&
                !board[fr][fc + 1].getPiece() && !targetExists)
                return true;
            if (colDiff == -1 && abs(rowDiff) == 1 && targetExists &&
                toCell.getPiece()->getColour() != colour)
                return true;
            break;
        }
    }

    return false;
}

bool Board::promotion(int fromRow, int fromCol, int toRow, int toCol, char tochar) {
    Cell &fromCell = board[fromRow][fromCol];
    Cell &toCell = board[toRow][toCol];

    Piece *piece = fromCell.getPiece();
    Colour colouradd;
    if (!piece) return false;
    if (piece->getSymbol() != 'P' && piece->getSymbol() != 'p') {
        return false;
    }
    if (piece->getSymbol() == 'p') {
        if (toRow - fromRow != 1 || toCol != fromCol) {
            return false;
        }
    }
    if (piece->getSymbol() == 'P') {
        if (fromRow - toRow != 1 || toCol != fromCol) {
            return false;
        }
    }
    Colour colour = board[fromRow][fromCol].getPiece()->getColour();
    if (tochar >= 'A' && tochar <= 'Z') {
        colouradd = Colour::White;
    } else if (tochar >= 'a' && tochar <= 'z') {
        colouradd = Colour ::Black;
    } else {
        return false;
    }
    if (colouradd != colour) return false;
    board[fromRow][fromCol].removePiece();
    std::unique_ptr<Piece> newPiece;
    if (tochar == 'Q' || tochar == 'q') {
        newPiece = make_unique<Queen>(colour);
    }else if (tochar == 'R' || tochar == 'r') {
        newPiece = make_unique<Rook>(colour);
    } else if (tochar == 'B' || tochar == 'b') {
        newPiece = make_unique<Bishop>(colour);
    } else if (tochar == 'N' || tochar == 'n') {
        newPiece = make_unique<Knight>(colour);
    } else {
        return false;
    }
    toCell.setPiece(std::move(newPiece));
    return true;
}

bool Board::promotion4(int fromRow, int fromCol, int toRow, int toCol, char tochar) {
    Cell &fromCell = board[fromRow][fromCol];
    Cell &toCell = board[toRow][toCol];

    Piece *piece = fromCell.getPiece();
    Colour colouradd;
    if (!piece) return false;
    if (piece->getType() != PieceType::PAWN) {
        return false;
    }
    Colour colour = board[fromRow][fromCol].getPiece()->getColour();
    if (colour == Colour::White) {
        if (toRow - fromRow != -1 || toCol != fromCol) {
            return false;
        }
    } else if (colour == Colour::Black) {
        if (toRow - fromRow != 1 || toCol != fromCol) {
            return false;
        }
    } else if (colour == Colour::Blue) {
        if (toRow != fromRow || toCol - fromCol != -1) {
            return false;
        }
    } else {
        if (toRow != fromRow || toCol - fromCol != 1) {
            return false;
        }
    }
    board[fromRow][fromCol].removePiece();
    std::unique_ptr<Piece> newPiece;
    switch (tochar) {
        case 'Q': 
            newPiece = std::make_unique<Queen>(colour);
            colouradd = Colour::White;
            break;
        case 'q':
            newPiece = std::make_unique<Queen>(colour);
            colouradd = Colour::Black;
            break;
        case 'S':
            newPiece = std::make_unique<Queen>(colour);
            colouradd = Colour::Red;
            break;
        case 's':
            newPiece = std::make_unique<Queen>(colour);
            colouradd = Colour::Blue;
            break;
        case 'R': 
            newPiece = std::make_unique<Queen>(colour);
            colouradd = Colour::White;
            break;
        case 'r':
            newPiece = std::make_unique<Queen>(colour);
            colouradd = Colour::Black;
            break;
        case 'D':
            newPiece = std::make_unique<Queen>(colour);
            colouradd = Colour::Red;
            break;
        case 'd':
            newPiece = std::make_unique<Queen>(colour);
            colouradd = Colour::Blue;
            break;
        case 'B': 
            newPiece = std::make_unique<Bishop>(colour);
            colouradd = Colour::White;
            break;
        case 'b':
            newPiece = std::make_unique<Bishop>(colour);
            colouradd = Colour::Black;
            break;
        case 'F':
            newPiece = std::make_unique<Bishop>(colour);
            colouradd = Colour::Red;
            break;
        case 'f':
            newPiece = std::make_unique<Bishop>(colour);
            colouradd = Colour::Blue;
            break;
        case 'N': 
            newPiece = std::make_unique<Knight>(colour);
            colouradd = Colour::White;
            break;
        case 'n':
            newPiece = std::make_unique<Knight>(colour);
            colouradd = Colour::Black;
            break;
        case 'G':
            newPiece = std::make_unique<Knight>(colour);
            colouradd = Colour::Red;
            break;
        case 'g':
            newPiece = std::make_unique<Knight>(colour);
            colouradd = Colour::Blue;
            break;
        case 'K': 
            newPiece = std::make_unique<King>(colour);
            colouradd = Colour::White;
            break;
        case 'k':
            newPiece = std::make_unique<King>(colour);
            colouradd = Colour::Black;
            break;
        case 'A':
            newPiece = std::make_unique<King>(colour);
            colouradd = Colour::Red;
            break;
        case 'a':
            newPiece = std::make_unique<King>(colour);
            colouradd = Colour::Blue;
            break;
        default:
            return false; // Invalid symbol
    }
    toCell.setPiece(std::move(newPiece));
    return true;
}

bool Board::checkKing(Colour colour) {
    int numKing = 0;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (board[row][col].getPiece()) {
                if (board[row][col].getPiece()->getType() == PieceType::KING) {
                    if (board[row][col].getPiece()->getColour() == colour) {
                        numKing++;
                    }
                } 
            }
        }
    } 
    if (numKing == 1) {
        return true;
    }
    return false;
}

bool Board::inbounds(int row, int col) {
    if (row >= 3 && row <= 10 && col >= 3 && col <= 10) return true;
    if (row >= 0 && row <= 2 && col >= 3 && col <= 10) return true;
    if (row >= 11 && row <= 13 && col >= 3 && col <= 10) return true;
    if (col >= 0 && col <= 2 && row >= 3 && row <= 10) return true;
    if (col >= 11 && col <= 13 && row >= 3 && row <= 10) return true;
    return false;
}

bool Board::checkPawn() {
    for (int col = 0; col < 8; col++) {
        if (board[1][col].getPiece() && board[7][col].getPiece()) {
            if (board[1][col].getPiece()->getType() == PieceType::PAWN 
            || board[7][col].getPiece()->getType() == PieceType::PAWN) {
                return false;
            }
        }
    }
    return true;
}

// getCell should be a member function of Board
Cell& Board::getCell(int row, int col) {
    return board[row][col];
}

const Cell& Board::getCell(int row, int col) const {
    return board[row][col];
}

std::ostream &operator<<(std::ostream &out, const Board &b) {
    if (b.td) {
        out << *b.td;
    }
    return out;
}
