#include "textdisplay.h"
#include "subject.h"
#include "info.h"
#include "piece.h"
using namespace std;

TextDisplay::TextDisplay(bool fourPlayer) : boardSize(fourPlayer ? 14 : 8) {
    td.resize(boardSize, std::vector<char>(boardSize, ' '));
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            if (fourPlayer && ((i < 3 && j < 3) || (i < 3 && j > 10) || (i > 10 && j < 3) || (i > 10 && j > 10))) {
                td[i][j] = '#';
            } else {
                td[i][j] = (i + j) % 2 == 0 ? '_' : ' ';
            }
        }
    }
}

void TextDisplay::notify(Subject &whoFrom) {
    Info pieceInfo = whoFrom.getInfo();
    int r = pieceInfo.row;
    int c = pieceInfo.col;
    if (r >= boardSize || c >= boardSize) return; // Bounds check
    // Prioritize holes in four-player mode
    if (boardSize == 14 && ((r < 3 && c < 3) || (r < 3 && c > 10) || 
                            (r > 10 && c < 3) || (r > 10 && c > 10))) {
        td[r][c] = '#';
    } else if (pieceInfo.piece) {
        td[r][c] = pieceInfo.piece->getSymbol();
    } else {
        td[r][c] = (r + c) % 2 == 0 ? '_' : ' ';
    }
}

TextDisplay::~TextDisplay() {}

std::ostream &operator<<(std::ostream &out, const TextDisplay &tdisp) {
    const auto &td = tdisp.td;
    const int size = tdisp.boardSize;

    // Print top border
    out << "  ";
    for (int j = 0; j < size; ++j) {
        out << "+---";
    }
    out << "+" << std::endl;

    // Print board rows with numbers
    for (int i = 0; i < size; ++i) {
        out << size - i << " ";
        if (size - i < 10) {
            out << " ";
        }
        for (int j = 0; j < size; ++j) {
            out << "| " << td[i][j] << " ";
        }
        out << "|" << std::endl;

        // Print row border
        out << "   ";
        for (int j = 0; j < size; ++j) {
            out << "+---";
        }
        out << "+" << std::endl;
    }

    // Print column labels
    out << "     ";
    for (int j = 0; j < size; ++j) {
        out << static_cast<char>('a' + j) << "   ";
    }
    out << std::endl << std::endl;

    return out;
}
