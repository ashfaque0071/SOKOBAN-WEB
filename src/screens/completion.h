#ifndef COMPLETION_H
#define COMPLETION_H

#include "raylib.h"
#include "assets.h"
#include "board.h"

typedef struct CompletionLayout {
    Rectangle panel;
    Rectangle nextButton;
    Rectangle replayButton;
    Rectangle homeButton;
} CompletionLayout;

CompletionLayout CompletionGetLayout(void);

void CompletionDraw(const Board *board, Assets *asset,
                    CompletionLayout layout,
                    bool nextHovered, bool replayHovered,
                    bool homeHovered, float elapsedTime);

void CompletionPlayStarTicks(const Board *board, float elapsedTime,
                             int *playedStarSounds);

#endif
