#ifndef BOARD_H
#define BOARD_H
#include <iostream>
#include <vector>
#include <cstddef>
#include "cell.h"
#include "info.h"
#include "piece.h"
#include <memory>
#include <utility>

class TextDisplay;
class GraphicsDisplay;

enum class Colour;
class Cell; 

class Board {
    std::vector<std::vector<Cell>> board;  // 8×8 grid of Cells
    std::shared_ptr<TextDisplay> td = nullptr;      // Text display observer
    std::shared_ptr<GraphicsDisplay> gd = nullptr;  // Graphics display observer
    struct EnPassantInfo {
        bool valid = false;
        int row = -1;
        int col = -1;
    } enPassant;
  public:
    bool whiteKingMoved = false, blackKingMoved = false;
    bool redKingMoved = false, blueKingMoved = false;
    bool whiteKingsideRookMoved = false, whiteQueensideRookMoved = false;
    bool blackKingsideRookMoved = false, blackQueensideRookMoved = false;
    bool redKingsideRookMoved = false, redQueensideRookMoved = false;
    bool blueKingsideRookMoved = false, blueQueensideRookMoved = false;
    Board();
    ~Board();
    bool isEnPassantTarget(int row, int col) const;
    // Initializes starting pieces on the board
    void init(bool fourPlayerMode);

    // Check if the path from (fromRow, fromCol) to (toRow, toCol) is clear (no pieces blocking)
    bool isPathClear(int fromRow, int fromCol, int toRow, int toCol) const;

    // Move piece from source to destination. Returns true if move was legal and successful.
    bool move(int fromRow, int fromCol, int toRow, int toCol);

    bool move4(int fromRow, int fromCol, int toRow, int toCol, const std::vector<Colour>& lostPlayers);

    // Returns true if the given colour is currently in check
    bool isInCheck(Colour colour) const;

    bool isInCheck4(Colour colour, const std::vector<Colour> &lostPlayers) const;

    // Returns true if the given colour is checkmated
    bool isCheckmate(Colour colour) const;

    void clear();

    bool fourplayermode() const;

    // Returns true if the given colour has any legal moves left
    bool hasAnyLegalMove(Colour colour);

    bool hasAnyLegalMove4(Colour colour, const std::vector<Colour> &lostPlayers);
    
    bool squareUnderAttack(int row, int col, Colour colour) const;

    std::pair<int, int> getPawnDirection(Colour c);

    bool pawnlegal4(int fr, int fc, int tr, int tc, Colour colour);

    bool pawnlegal(int fromRow, int fromCol, int toRow, int toCol, Colour colour);

    bool promotion(int fromRow, int fromCol, int toRow, int toCol, char tochar);

    bool promotion4(int fromRow, int fromCol, int toRow, int toCol, char tochar);

    bool checkKing(Colour colour);
    
    bool checkPawn();

    bool inbounds(int row, int col);

    // Returns reference to cell at given position
    Cell& getCell(int row, int col);
    const Cell& getCell(int row, int col) const;

    // Print board using TextDisplay
    friend std::ostream &operator<<(std::ostream &out, const Board &b);
};

std::ostream &operator<<(std::ostream &out, const Board &b);
#endif
