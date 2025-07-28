#ifndef GRAPHICSDISPLAY_H
#define GRAPHICSDISPLAY_H

#include <vector>
#include "observer.h"
#include "info.h"
#include "window.h"
#include "subject.h"

class GraphicsDisplay : public Observer {
    Xwindow xw;
    const int gridSize;
    const int windowSize;
    const int cellSize;

public:
    GraphicsDisplay(bool fourPlayer);
    void notify(Subject &whoFrom) override;
    ~GraphicsDisplay() = default;
    void drawLabels();
};

#endif
