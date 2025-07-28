#include "game.h"
#include "player.h"
#include "piece.h"
#include "bishop.h"
#include "king.h"
#include "knight.h"
#include "pawn.h"
#include "queen.h"
#include "rook.h"
// #include "computerplayer.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

using namespace std;

pair <int, int> convertCoord(const string &coord, const Game &game) { // to convert 'D2' into the corresponding row and col
    if (game.fourPlayer) {
        if (coord.length() < 2 || coord.length() > 3) {
            throw invalid_argument("Invalid coordinate: " + coord);
        }

        char file = tolower(coord[0]);
        if (file < 'a' || file > 'n') { // 'a' to 'n' for 14 columns
            throw invalid_argument("Invalid file (column): " + coord);
        }

        // Manually parse the rank string into an integer (no stoi)
        int rank = 0;
        for (size_t i = 1; i < coord.length(); ++i) {
            char c = coord[i];
            if (c < '0' || c > '9') {
                throw invalid_argument("Invalid digit in rank: " + coord);
            }
            rank = rank * 10 + (c - '0');
        }

        if (rank < 1 || rank > 14) {
            throw invalid_argument("Rank out of bounds: " + coord);
        }

        int col = file - 'a';     // 0-indexed column
        int row = 14 - rank;      // 0-indexed row (top is 0)

        return {row, col};

    }
    if (coord.length() != 2 || coord[0] < 'a' || coord[0] > 'h' || coord[1] < '1' || coord[1] > '8') {
        throw invalid_argument("Invalid coordinate: " + coord);
    }
    int col = coord[0] - 'a';
    int row = 8 - (coord[1] - '0');
    return {row, col};
}

Game::Game() : isGameOver(false), enteredsetup(false), fourPlayer(false),
               whiteScore(0), blackScore(0), redScore(0), blueScore(0),
               currentTurn(Colour::White) {
    lostPlayers.clear();
}


Game::~Game() = default;

Board& Game::getBoard() {
    return board;
}

const Board& Game::getBoard() const {
    return board;
}

void Game::printBoard() const {
    cout << board;
}

void Game::start(const string &whiteType, const string &blackType) {
    if (startedbefore) board.clear();
    startedbefore = true;
    if (!enteredsetup) {
        if (!fourPlayer) {
            board.init(fourPlayer);
            board.getCell(0, 0).setPiece(make_unique<Rook>(Colour::Black));
            board.getCell(0, 1).setPiece(make_unique<Knight>(Colour::Black));
            board.getCell(0, 2).setPiece(make_unique<Bishop>(Colour::Black));
            board.getCell(0, 3).setPiece(make_unique<Queen>(Colour::Black));
            board.getCell(0, 4).setPiece(make_unique<King>(Colour::Black));
            board.getCell(0, 5).setPiece(make_unique<Bishop>(Colour::Black));
            board.getCell(0, 6).setPiece(make_unique<Knight>(Colour::Black));
            board.getCell(0, 7).setPiece(make_unique<Rook>(Colour::Black));
            for (int c = 0; c < 8; ++c) {
                board.getCell(1, c).setPiece(make_unique<Pawn>(Colour::Black));
            }

            // Row 6 and 7: White
            for (int c = 0; c < 8; ++c) {
                board.getCell(6, c).setPiece(make_unique<Pawn>(Colour::White));
            }
            board.getCell(7, 0).setPiece(make_unique<Rook>(Colour::White));
            board.getCell(7, 1).setPiece(make_unique<Knight>(Colour::White));
            board.getCell(7, 2).setPiece(make_unique<Bishop>(Colour::White));
            board.getCell(7, 3).setPiece(make_unique<Queen>(Colour::White));
            board.getCell(7, 4).setPiece(make_unique<King>(Colour::White));
            board.getCell(7, 5).setPiece(make_unique<Bishop>(Colour::White));
            board.getCell(7, 6).setPiece(make_unique<Knight>(Colour::White));
            board.getCell(7, 7).setPiece(make_unique<Rook>(Colour::White));
        } else {
            board.init(fourPlayer);
            board.getCell(0, 3).setPiece(make_unique<Rook>(Colour::Black));
            board.getCell(0, 4).setPiece(make_unique<Knight>(Colour::Black));
            board.getCell(0, 5).setPiece(make_unique<Bishop>(Colour::Black));
            board.getCell(0, 6).setPiece(make_unique<Queen>(Colour::Black));
            board.getCell(0, 7).setPiece(make_unique<King>(Colour::Black));
            board.getCell(0, 8).setPiece(make_unique<Bishop>(Colour::Black));
            board.getCell(0, 9).setPiece(make_unique<Knight>(Colour::Black));
            board.getCell(0, 10).setPiece(make_unique<Rook>(Colour::Black));
            for (int c = 3; c < 11; ++c) {
                board.getCell(1, c).setPiece(make_unique<Pawn>(Colour::Black));
            }

            // White (Bottom)
            for (int c = 3; c < 11; ++c) {
                board.getCell(12, c).setPiece(make_unique<Pawn>(Colour::White));
            }
            board.getCell(13, 3).setPiece(make_unique<Rook>(Colour::White));
            board.getCell(13, 4).setPiece(make_unique<Knight>(Colour::White));
            board.getCell(13, 5).setPiece(make_unique<Bishop>(Colour::White));
            board.getCell(13, 6).setPiece(make_unique<Queen>(Colour::White));
            board.getCell(13, 7).setPiece(make_unique<King>(Colour::White));
            board.getCell(13, 8).setPiece(make_unique<Bishop>(Colour::White));
            board.getCell(13, 9).setPiece(make_unique<Knight>(Colour::White));
            board.getCell(13, 10).setPiece(make_unique<Rook>(Colour::White));

            // Red (Left)
            for (int r = 3; r < 11; ++r) {
                board.getCell(r, 1).setPiece(make_unique<Pawn>(Colour::Red));
            }
            board.getCell(3, 0).setPiece(make_unique<Rook>(Colour::Red));
            board.getCell(4, 0).setPiece(make_unique<Knight>(Colour::Red));
            board.getCell(5, 0).setPiece(make_unique<Bishop>(Colour::Red));
            board.getCell(6, 0).setPiece(make_unique<Queen>(Colour::Red));
            board.getCell(7, 0).setPiece(make_unique<King>(Colour::Red));
            board.getCell(8, 0).setPiece(make_unique<Bishop>(Colour::Red));
            board.getCell(9, 0).setPiece(make_unique<Knight>(Colour::Red));
            board.getCell(10, 0).setPiece(make_unique<Rook>(Colour::Red));

            // Blue (Right)
            for (int r = 3; r < 11; ++r) {
                board.getCell(r, 12).setPiece(make_unique<Pawn>(Colour::Blue));
            }
            board.getCell(3, 13).setPiece(make_unique<Rook>(Colour::Blue));
            board.getCell(4, 13).setPiece(make_unique<Knight>(Colour::Blue));
            board.getCell(5, 13).setPiece(make_unique<Bishop>(Colour::Blue));
            board.getCell(6, 13).setPiece(make_unique<Queen>(Colour::Blue));
            board.getCell(7, 13).setPiece(make_unique<King>(Colour::Blue));
            board.getCell(8, 13).setPiece(make_unique<Bishop>(Colour::Blue));
            board.getCell(9, 13).setPiece(make_unique<Knight>(Colour::Blue));
            board.getCell(10, 13).setPiece(make_unique<Rook>(Colour::Blue));

        }
    }

    isGameOver = false;   // there is one game running
    currentTurn = Colour::White;        // white starts first
    
    if (!fourPlayer) {
        // initialize white player
        if (whiteType == "human") {
            whitePlayer = make_unique<HumanPlayer>(Colour::White);
        }
        else if (whiteType.substr(0,8) == "computer") {
            int level = whiteType[8] - '0';
            if (level == 1) {
                whitePlayer = make_unique<Level1Computer>(Colour::White);
            }
            else if (level == 2) {
                whitePlayer = make_unique<Level2Computer>(Colour::White);
            }
            else if (level == 3) {
                whitePlayer = make_unique<Level3Computer>(Colour::White);
            }
            else if (level == 4) {
                whitePlayer = make_unique<Level4Computer>(Colour::White);
            }
        }

        // initialize black player
        if (blackType == "human") {
            blackPlayer = make_unique<HumanPlayer>(Colour::Black);
        }
        else if (blackType.substr(0,8) == "computer") {
            int level = blackType[8] - '0';
            if (level == 1) {
                blackPlayer = make_unique<Level1Computer>(Colour::Black);
            }
            else if (level == 2) {
                blackPlayer = make_unique<Level2Computer>(Colour::Black);
            }
            else if (level == 3) {
                blackPlayer = make_unique<Level3Computer>(Colour::Black);
            }
            else if (level == 4) {
                blackPlayer = make_unique<Level4Computer>(Colour::Black);
            }
        }
    }

    else {
        whitePlayer = make_unique<HumanPlayer>(Colour::White);
        blackPlayer = make_unique<HumanPlayer>(Colour::Black);
        redPlayer = make_unique<HumanPlayer>(Colour::Red);
        bluePlayer = make_unique<HumanPlayer>(Colour::Blue);
    }
    printBoard();
}

void Game::move(const string &from, const string &to, const string &promo) {
    if (isGameOver) {
        cerr << "No game at the moment" << endl;
        return;
    }

    Player* currentPlayer = nullptr;
    if (currentTurn == Colour::White) currentPlayer = whitePlayer.get();
    else if (currentTurn == Colour::Black) currentPlayer = blackPlayer.get();
    else if (currentTurn == Colour::Red) currentPlayer = redPlayer.get();
    else if (currentTurn == Colour::Blue) currentPlayer = bluePlayer.get();
    if (!currentPlayer) {
        cerr << "No player for current turn" << endl;
        return;
    }

    // Case 1: Computer move in 1v1 mode
    if (!fourPlayer && from.empty() && to.empty() && currentPlayer->getPlayerType().find("computer") != string::npos) {
        Move move = currentPlayer->makeMove(board, "", "", "");
        if (move.fromRow == -1) {
            cerr << "No legal moves available." << endl;
            return;
        }
        cerr << "Computer (" << (currentTurn == Colour::White ? "White" : "Black")
             << ") moves: " << move.toString() << endl;
    }
    // Case 2: Human move
    else if (!from.empty() && !to.empty() && currentPlayer->getPlayerType() == "human") {
        try {
            auto fromCoord = convertCoord(from, *this);
            auto toCoord = convertCoord(to, *this);
            Piece* piece = board.getCell(fromCoord.first, fromCoord.second).getPiece();
            if (!piece) {
                cerr << "No piece at " << from << endl;
                return;
            }
            if (piece->getColour() != currentTurn) {
                cerr << "Not your piece" << endl;
                return;
            }
            bool success;
            if (fourPlayer) {
                if (!promo.empty()) {
                    bool validPromotion = false;
                    switch (piece->getColour()) {
                        case Colour::White:
                            if (toCoord.first == 6) validPromotion = true; // Reached top
                            break;
                        case Colour::Black:
                            if (toCoord.first == 7) validPromotion = true; // Reached bottom
                            break;
                        case Colour::Red:
                            if (toCoord.second == 7) validPromotion = true; // Reached right
                            break;
                        case Colour::Blue:
                            if (toCoord.second == 6) validPromotion = true; // Reached left
                            break;
                    }
                    if (validPromotion) {
                        success = board.promotion4(fromCoord.first, fromCoord.second,
                                         toCoord.first, toCoord.second, promo[0]);
                    }
                } else { 
                    success = board.move4(fromCoord.first, fromCoord.second,
                                    toCoord.first, toCoord.second, lostPlayers);
                }
            } else {
                if (!promo.empty()) {
                    if (toCoord.first == 0 || toCoord.second == 7) {
                    success = board.promotion(fromCoord.first, fromCoord.second,
                                            toCoord.first, toCoord.second, promo[0]);
                    }
                                            
                } else { 
                    success = board.move(fromCoord.first, fromCoord.second,
                                        toCoord.first, toCoord.second);
                }
            }
            if (!success) {
                cerr << "Illegal move" << endl;
                return;
            }
            cerr << "Human (" << (currentTurn == Colour::White ? "White" :
                                  currentTurn == Colour::Black ? "Black" :
                                  currentTurn == Colour::Red ? "Red" : "Blue")
                 << ") moves: " << from << to << promo << endl;
        }
        catch (const invalid_argument& e) {
            cerr << "Invalid move format" << endl;
            return;
        }
    }
    else {
        cerr << "Invalid command. Use 'move' for computer or 'move <from> <to>' for human." << endl;
        return;
    }
    printBoard();
    // 1v1 mode
    if (!fourPlayer) {
        Colour opponent = (currentTurn == Colour::White) ? Colour::Black : Colour::White;
        if (!board.hasAnyLegalMove(opponent)) {
            cout << "ruby chan" << endl;
            fourPlayer = false;
            enteredsetup = false;
            if (board.isInCheck(opponent)) {
                cout << "Checkmate! " << (currentTurn == Colour::White ? "White" : "Black") << " wins!" << endl;
                (currentTurn == Colour::White ? whiteScore : blackScore) += 1;
            } else {
                fourPlayer = false;
                cout << "Stalemate!" << endl;
                whiteScore += 0.5;
                blackScore += 0.5;
            }
            isGameOver = true;
        }
        else if (board.isInCheck(opponent)) {
            cout << (opponent == Colour::White ? "White" : "Black") << " is in check." << endl;
        } else {
            currentTurn = opponent;
        }
        return;
    }

    // Four-player mode
    // Check for players in check
    string checkMsg = "";
    for (Colour c : {Colour::White, Colour::Black, Colour::Red, Colour::Blue}) {
        if (c != currentTurn && board.isInCheck4(c, lostPlayers) &&
            find(lostPlayers.begin(), lostPlayers.end(), c) == lostPlayers.end()) {
            checkMsg += (c == Colour::White ? "White " :
                         c == Colour::Black ? "Black " :
                         c == Colour::Red ? "Red " : "Blue ");
        }
    }
    if (!checkMsg.empty()) cout << checkMsg << "is in check." << endl;

    // Count kings
    int kingCount[4] = {0, 0, 0, 0}; // White, Black, Red, Blue
    int boardSize = fourPlayer ? 14 : 8;
    for (int r = 0; r < boardSize; ++r) {
        for (int c = 0; c < boardSize; ++c) {
            Piece* p = board.getCell(r, c).getPiece();
            if (p && p->getType() == PieceType::KING) {
                if (p->getColour() == Colour::White) kingCount[0]++;
                else if (p->getColour() == Colour::Black) kingCount[1]++;
                else if (p->getColour() == Colour::Red) kingCount[2]++;
                else if (p->getColour() == Colour::Blue) kingCount[3]++;
            }
        }
    }
    // Check for checkmate/stalemate for all players
    for (Colour c : {Colour::White, Colour::Black, Colour::Red, Colour::Blue}) {
        if (find(lostPlayers.begin(), lostPlayers.end(), c) != lostPlayers.end()) continue;
        if (kingCount[c == Colour::White ? 0 : c == Colour::Black ? 1 : c == Colour::Red ? 2 : 3] == 0) {
            lostPlayers.push_back(c);
            cout << (c == Colour::White ? "White" : c == Colour::Black ? "Black" :
                     c == Colour::Red ? "Red" : "Blue") << " has lost (king captured)!" << endl;
        } else if (!board.hasAnyLegalMove4(c, lostPlayers)) {
            lostPlayers.push_back(c);
            if (board.isInCheck4(c, lostPlayers)) {
                cout << (c == Colour::White ? "White" : c == Colour::Black ? "Black" :
                         c == Colour::Red ? "Red" : "Blue") << " is checkmated!" << endl;
            } else {
                cout << (c == Colour::White ? "White" : c == Colour::Black ? "Black" :
                         c == Colour::Red ? "Red" : "Blue") << " is stalemated!" << endl;
            }
        }
    }

    // Check win condition: one player left
    int activePlayers = 4 - static_cast<int>(lostPlayers.size());
    if (activePlayers <= 1) {
        enteredsetup = false;
        isGameOver = true;
        if (activePlayers == 1) {
            Colour winner = Colour::Draw;
            for (Colour c : {Colour::White, Colour::Black, Colour::Red, Colour::Blue}) {
                if (find(lostPlayers.begin(), lostPlayers.end(), c) == lostPlayers.end()) {
                    winner = c;
                    break;
                }
            }
            cout << "Game Over! " << (winner == Colour::White ? "White" :
                                     winner == Colour::Black ? "Black" :
                                     winner == Colour::Red ? "Red" : "Blue") << " wins!" << endl;
            fourPlayer = false;
            if (winner == Colour::White) whiteScore += 1;
            else if (winner == Colour::Black) blackScore += 1;
            else if (winner == Colour::Red) redScore += 1;
            else if (winner == Colour::Blue) blueScore += 1;
        } else {
            cout << "Game Over! Draw (all remaining players stalemated)!" << endl;
            fourPlayer = false;
            for (Colour c : {Colour::White, Colour::Black, Colour::Red, Colour::Blue}) {
                if (find(lostPlayers.begin(), lostPlayers.end(), c) == lostPlayers.end()) {
                    if (c == Colour::White) whiteScore += 0.25;
                    else if (c == Colour::Black) blackScore += 0.25;
                    else if (c == Colour::Red) redScore += 0.25;
                    else if (c == Colour::Blue) blueScore += 0.25;
                }
            }
        }
        return;
    }

    // Next turn: skip lost players
    Colour nextTurn = currentTurn;
    int attempts = 0;
    do {
        if (nextTurn == Colour::White) nextTurn = Colour::Red;
        else if (nextTurn == Colour::Red) nextTurn = Colour::Black;
        else if (nextTurn == Colour::Black) nextTurn = Colour::Blue;
        else if (nextTurn == Colour::Blue) nextTurn = Colour::White;
        attempts++;
        if (attempts > 4) {
            cout << "Error: No active players found!" << endl;
            isGameOver = true;
            return;
        }
    } while (find(lostPlayers.begin(), lostPlayers.end(), nextTurn) != lostPlayers.end());
    currentTurn = nextTurn;
}


void Game::resign() {
    enteredsetup = false;
    board.whiteKingMoved = false; 
    board.blackKingMoved = false;
    board.redKingMoved = false; 
    board.blueKingMoved = false;
    board.whiteKingsideRookMoved = false;
    board.whiteQueensideRookMoved = false;
    board.blackKingsideRookMoved = false;
    board.blackQueensideRookMoved = false;
    board.redKingsideRookMoved = false;
    board.redQueensideRookMoved = false;
    board.blueKingsideRookMoved = false;
    board.blueQueensideRookMoved = false;
    fourPlayer = false;
    if (isGameOver) {
        cerr << "No game in progress" << endl;
        return;
    }
    cerr << (currentTurn == Colour::White ? "Black" : "White") << " wins by resignation" << endl;
    if (currentTurn == Colour::White) {
        blackScore += 1;
    } 
    else {
        whiteScore += 1;
    }
    isGameOver = true;
}

void Game::getScore() const {
    cout << "Final Score:" << endl;
    cout << "White: " << whiteScore << "\n";
    cout << "Black: " << blackScore << "\n";
}

void Game::setup() {
    startedbefore = true;
    enteredsetup = true;
    isGameOver = true;
    board.init(false);
    board.clear();
}

bool Game::isOver() const {
    return isGameOver;
}


bool Game::setTurn(const Colour &colour) {
    int whiteKingCount = 0;
    int blackKingCount = 0;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece *p = board.getCell(row, col).getPiece();
            if (!p) continue;
            
            char sym = p->getSymbol();
            if (sym == 'K') ++whiteKingCount;
            else if (sym == 'k') ++blackKingCount;

            // No pawns on row 0 or 7
            if ((sym == 'P' || sym == 'p') && (row == 0 || row == 7)) {
                return false;
            }
        }
    }

    if (whiteKingCount != 1 || blackKingCount != 1) return false;
    if (board.isInCheck(Colour::White) || board.isInCheck(Colour::Black)) return false;
    currentTurn = colour;
    return true;
}


