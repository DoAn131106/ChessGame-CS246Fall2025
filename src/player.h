#ifndef PLAYER_H
#define PLAYER_H
#include "piece.h"
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Board;


class Player {
protected:
  Colour colour;

public:
  Player(Colour c);
  virtual ~Player() = default;
  Colour getColour() const;
  virtual Move makeMove(Board &board, const std::string &from = "",
                        const std::string &to = "",
                        const std::string &promo = "") = 0;
  virtual std::string getPlayerType() const = 0;
};

class HumanPlayer : public Player {
public:
  HumanPlayer(Colour c);
  Move makeMove(Board &board, const std::string &from = "",
                const std::string &to = "",
                const std::string &promo = "") override;
  std::string getPlayerType() const override;;
};

class ComputerPlayer : public Player {
private:
  virtual int evaluateBoard(const Board &board) const = 0;
  virtual Move selectMove(const Board &board) const = 0;

protected:
  int difficulty;
  int getPieceValue(PieceType piece) const;

public:
  ComputerPlayer(Colour c, int diff);
  Move makeMove(Board &board, const std::string &from = "",
                const std::string &to = "",
                const std::string &promo = "") override;
  // virtual bool makeMove(Board& board) = 0;
  std::string getPlayerType() const override;
  int getDifficulty() const;
  void addMoveWithPromotions(std::vector<Move>& legalMoves, const Board& board, 
    int fromRow, int fromCol, int toRow, int toCol) const;
  std::vector<Move> getAllLegalMoves(const Board &board) const;
};

class Level1Computer : public ComputerPlayer {
protected:
  int evaluateBoard(const Board &board) const override;
  Move selectMove(const Board &board) const override;

public:
  Level1Computer(Colour c);
  Move makeMove(Board &board, const std::string &from = "",
                const std::string &to = "",
                const std::string &promo = "") override;
};

class Level2Computer : public ComputerPlayer {
protected:
  int evaluateBoard(const Board &board) const override;
  Move selectMove(const Board &board) const override;
  std::vector<Move> getCapturesMoves(const std::vector<Move> &moves,
                                     const Board &board) const;
  std::vector<Move> getCheckMoves(const std::vector<Move> &moves,
                                  const Board &board) const;

public:
  Level2Computer(Colour c);
  Move makeMove(Board &board, const std::string &from = "",
                const std::string &to = "",
                const std::string &promo = "") override;
};

class Level3Computer : public ComputerPlayer {
protected:
  int evaluateBoard(const Board &board) const override;
  Move selectMove(const Board &board) const override;
  std::vector<Move> getSafeMoves(const std::vector<Move> &moves,
                                 const Board &board) const;
  bool isMoveSafe(const Board &board, const Move &move) const;
  int evaluateMoveScore(const Board &board, const Move &move) const;

public:
  Level3Computer(Colour c);
  Move makeMove(Board &board, const std::string &from = "",
                const std::string &to = "",
                const std::string &promo = "") override;
};

class Level4Computer : public ComputerPlayer {
protected:
  static const int MAX_DEPTH = 3;
  int evaluateBoard(const Board &board) const override;
  Move selectMove(const Board &board) const override;
  int minimax(Board board, int depth, bool isMaximizing, int alpha,
              int beta) const;
  int evaluatePosition(const Board &board) const;
  int countLegalMoves(const Board& board, Colour color) const;

public:
  Level4Computer(Colour c);
  Move makeMove(Board &board, const std::string &from = "",
                const std::string &to = "",
                const std::string &promo = "") override;
};

#endif
