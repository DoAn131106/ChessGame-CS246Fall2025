#ifndef GAME_H
#define GAME_H

#include <string>
#include <memory>
#include <vector>
#include "board.h"
#include "player.h"

class Game {
    Board board;
    std::unique_ptr<Player> whitePlayer;
    std::unique_ptr<Player> blackPlayer;
    std::unique_ptr<Player> redPlayer;
    std::unique_ptr<Player> bluePlayer;
    Colour currentTurn;
    bool isGameOver = true;
    double whiteScore = 0;
    double blackScore = 0;
    double redScore = 0;
    double blueScore = 0;
    std::vector<Colour> lostPlayers;
 public:
    bool fourPlayer = false;
    bool startedbefore = false;
    bool enteredsetup = false;
    Game();
    ~Game();
    void start(const std::string &whiteType, const std::string &blackType);
    void move(const std::string &from, const std::string &to, const std::string &promo = "");
    void resign();
    void setup();
    void printBoard() const;
    void getScore() const;
    bool isOver() const;
    bool setTurn(const Colour &colour);
    Board& getBoard();
    const Board& getBoard() const;
};

#endif
