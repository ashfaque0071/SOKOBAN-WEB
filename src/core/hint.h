#ifndef HINT_H
#define HINT_H

#include "raylib.h"
#include "assets.h"
#include "board.h"

typedef enum HintStatus {
    HINT_IDLE,
    HINT_READY,
    HINT_DEAD,
    HINT_SOLVED
} HintStatus;

typedef struct Hint {
    HintStatus status;
    bool active;
    int pushesLeft;
    int boxRow, boxCol;
    int pushRow, pushCol;
    int standRow, standCol;
} Hint;

Hint HintAsk(const Board *board);

void HintDraw(const Hint *hint, int originX, int originY, int tile);

void HintDrawDeadNotice(Assets *asset, Rectangle area);

void HintUnload(void);

#endif
