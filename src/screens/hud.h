#ifndef HUD_H
#define HUD_H

#include "raylib.h"
#include "assets.h"
#include "board.h"
#include "hint.h"

typedef struct HudLayout {
    Rectangle statsPanel;
    Rectangle levelPanel;
    Rectangle movePanel;
    Rectangle pushPanel;
    Rectangle bestPanel;
    Rectangle hintButton;
    Rectangle undoButton;
    Rectangle restartButton;
    Rectangle pauseButton;
    Rectangle boardFrame;
    int originX;
    int originY;
    int tile;
    int panelY;
    int panelHeight;
    int infoFont;
    int controlsX;
} HudLayout;

typedef struct HudHover {
    bool hint;
    bool undo;
    bool restart;
    bool pause;
} HudHover;

HudLayout HudGetLayout(void);

void HudDraw(const Board *board, Assets *asset, const HudLayout *layout,
             const Hint *hint, HudHover hover);

#endif
