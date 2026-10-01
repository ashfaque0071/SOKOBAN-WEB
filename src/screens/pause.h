#ifndef PAUSE_H
#define PAUSE_H

#include "raylib.h"
#include "assets.h"
#include "board.h"

typedef struct PauseLayout {
    Rectangle panel;
    Rectangle resumeButton;
    Rectangle restartButton;
    Rectangle settingsButton;
    Rectangle homeButton;
} PauseLayout;

PauseLayout PauseGetLayout(Assets *asset);
void PauseDraw(const Board *board, Assets *asset, const PauseLayout *layout,
               bool resumeHovered, bool restartHovered,
               bool settingsHovered, bool homeHovered);

#endif
