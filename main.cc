#include <iostream>
#include <string>
#include <sstream>
#include "game.h"
#include "piece.h"
#include "king.h"
#include "queen.h"
#include "rook.h"
#include "bishop.h"
#include "pawn.h"
#include "knight.h"

using namespace std;

unique_ptr<Piece> createPiece(char symbol, bool fourPlayer) {
    if (!fourPlayer) {
        // 2-player mode: White (uppercase) and Black (lowercase)
        Colour colour = (symbol >= 'A' && symbol <= 'Z') ? Colour::White : Colour::Black;
        symbol = tolower(symbol); // Normalize to lowercase for piece type
        if (symbol == 'k') return make_unique<King>(colour);
        if (symbol == 'q') return make_unique<Queen>(colour);
        if (symbol == 'r') return make_unique<Rook>(colour);
        if (symbol == 'b') return make_unique<Bishop>(colour);
        if (symbol == 'p') return make_unique<Pawn>(colour);
        if (symbol == 'n') return make_unique<Knight>(colour);
    } else {
        // 4-player mode: White, Black, Red, Blue
        if (symbol == 'K') return make_unique<King>(Colour::White);
        if (symbol == 'Q') return make_unique<Queen>(Colour::White);
        if (symbol == 'R') return make_unique<Rook>(Colour::White);
        if (symbol == 'B') return make_unique<Bishop>(Colour::White);
        if (symbol == 'N') return make_unique<Knight>(Colour::White);
        if (symbol == 'P') return make_unique<Pawn>(Colour::White);
        if (symbol == 'k') return make_unique<King>(Colour::Black);
        if (symbol == 'q') return make_unique<Queen>(Colour::Black);
        if (symbol == 'r') return make_unique<Rook>(Colour::Black);
        if (symbol == 'b') return make_unique<Bishop>(Colour::Black);
        if (symbol == 'n') return make_unique<Knight>(Colour::Black);
        if (symbol == 'p') return make_unique<Pawn>(Colour::Black);
        if (symbol == 'A') return make_unique<King>(Colour::Red);
        if (symbol == 'S') return make_unique<Queen>(Colour::Red);
        if (symbol == 'D') return make_unique<Rook>(Colour::Red);
        if (symbol == 'F') return make_unique<Bishop>(Colour::Red);
        if (symbol == 'G') return make_unique<Knight>(Colour::Red);
        if (symbol == 'H') return make_unique<Pawn>(Colour::Red);
        if (symbol == 'a') return make_unique<King>(Colour::Blue);
        if (symbol == 's') return make_unique<Queen>(Colour::Blue);
        if (symbol == 'd') return make_unique<Rook>(Colour::Blue);
        if (symbol == 'f') return make_unique<Bishop>(Colour::Blue);
        if (symbol == 'g') return make_unique<Knight>(Colour::Blue);
        if (symbol == 'h') return make_unique<Pawn>(Colour::Blue);
    }
    return nullptr; // Invalid symbol
}



pair <int, int> convertCoord1(const string &coord, const Game &game) { // to convert 'D2' into the corresponding row and col
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

int main() {
    Game game;
    string line;
    while (getline(cin, line)) {
        istringstream iss(line);
        string cmd;
        iss >> cmd;
        if (cmd == "gamefour") {    // assume that game four has all players as human
            game.fourPlayer = true;
            game.start("human", "human");
        }
        else if (cmd == "game") {    // starting a new game
            string whiteType, blackType;
            iss >> whiteType;
            iss >> blackType;
            game.start(whiteType, blackType);
        }

        else if (cmd == "resign") {  // one player resigns
            game.resign();
        }

        else if (cmd == "move") {    // a move is made
            string from, to, promoteTo;
            iss >> from >> to >> promoteTo;
            game.move(from, to, promoteTo);      // logic of promotion is included in move() of game
        }

        else if (cmd == "print") {
            game.printBoard();
        }

        else if (cmd == "score") {
            game.getScore();
        }

        else if (cmd == "setup" && !game.fourPlayer) {
            game.enteredsetup = true;
            game.setup();
            if (game.isOver()) {
                while (getline(cin, line)) {
                    istringstream iss{line};
                    string setupCmd;
                    iss >> setupCmd;
                    if (setupCmd == "done") {   // the command is "done"
                        if (game.getBoard().checkKing(Colour::White) && game.getBoard().checkKing(Colour::Black) 
                        && game.getBoard().checkPawn() && game.getBoard().isInCheck(Colour::White) && game.getBoard().isInCheck(Colour::Black)) break;
                        else { cerr << "Do not meet requirements to start game" << endl; };
                    } 
                    if (setupCmd == "+") {      // the command must be + K e1 for example
                        string symbol, cell;
                        iss >> symbol >> cell;
                        auto cellInCoord = convertCoord1(cell, game);
                        if (symbol.length() != 1) {
                            std::cerr << "Invalid piece symbol: " << symbol << std::endl;
                            continue; // Skip invalid input
                        }
                        auto piece = createPiece(symbol[0], game.fourPlayer);
                        if (piece) {
                            game.getBoard().getCell(cellInCoord.first, cellInCoord.second).setPiece(std::move(piece));
                        }
                    }
                    else if (setupCmd == "-") {
                        string cell;
                        iss >> cell;
                        auto cellInCoord = convertCoord1(cell, game);
                        game.getBoard().getCell(cellInCoord.first, cellInCoord.second).removePiece();
                    }
                    else if (setupCmd == "=") {
                        string colour;
                        iss >> colour;
                        game.setTurn(colour == "white" ? Colour::White : Colour::Black);
                    }
                }
            }
            game.printBoard();
            }
        else {
            cerr << "Invalid move" << endl;
        }
    }
    game.getScore();
    return 0;
}
