#ifndef LEVELSELECT_H
#define LEVELSELECT_H

#include "raylib.h"
#include "assets.h"
#include "board.h"

#define LEVEL_COLUMNS 4
#define LEVEL_SLOTS 16

#define LEVEL_SHELF_HIGH_SCORES 0
#define LEVEL_SHELF_CHEATSHEETS 1

typedef struct LevelSelectLayout {
    Rectangle card[LEVEL_SLOTS];
    Rectangle banner;
    Rectangle starTag;
    Rectangle backButton;
    Rectangle controlsButton;
    Rectangle highScoresButton;
    Rectangle cheatsheetsButton;
} LevelSelectLayout;

typedef struct LevelSelectHover {
    int card;
    int lockedCard;
    int shelf;
    bool back;
    bool controls;
} LevelSelectHover;

LevelSelectLayout LevelSelectGetLayout(Assets *asset);

int LevelSelectSlotCount(void);
LevelSelectHover LevelSelectHitTest(const LevelSelectLayout *layout,
                                    Vector2 mouse);

bool LevelSelectHoveringControl(LevelSelectHover hover);
void LevelSelectDraw(const Board *board, Assets *asset,
                     const LevelSelectLayout *layout,
                     LevelSelectHover hover);

#endif
