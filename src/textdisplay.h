#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H

#include <iostream>
#include <vector>
#include "observer.h"
#include "info.h"

using namespace std;

class TextDisplay : public Observer {
    std::vector<std::vector<char>> td;
    int boardSize; // To store 8 or 14 based on fourPlayer mode
 public:
    TextDisplay(bool fourPlayer); // Constructor with fourPlayer parameter
    ~TextDisplay();
    void notify(Subject &whoFrom) override;
    friend std::ostream &operator<<(std::ostream &out, const TextDisplay &td);
};

#endif
