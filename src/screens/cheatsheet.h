#ifndef CHEATSHEET_H
#define CHEATSHEET_H

#include "raylib.h"
#include "assets.h"
#include "board.h"

#include "score.h"
#include "ui.h"

#define CHEAT_MAX_STEPS 512

typedef enum CheatDirection {
    CHEAT_UP,
    CHEAT_DOWN,
    CHEAT_LEFT,
    CHEAT_RIGHT
} CheatDirection;

typedef struct CheatStep {
    unsigned char direction;
    unsigned short repeat;
} CheatStep;

typedef struct CheatRoute {
    CheatStep step[CHEAT_MAX_STEPS];
    int stepCount;
    int optimalMoves;
    int optimalPushes;
} CheatRoute;

typedef struct CheatsheetState {
    int level;
    bool revealed;
    int page;
} CheatsheetState;

typedef struct CheatsheetLayout {
    Rectangle screen;
    Rectangle title;
    Rectangle levelPlate;
    Rectangle note;
    Rectangle panel;
    Rectangle paper;
    Rectangle prevTab;
    Rectangle nextTab;
    Rectangle backButton;
    Rectangle revealButton;
    Rectangle pagePrev;
    Rectangle pageNext;
} CheatsheetLayout;

typedef enum CheatsheetControl {
    CHEAT_CONTROL_NONE = -1,
    CHEAT_CONTROL_PREV,
    CHEAT_CONTROL_NEXT,
    CHEAT_CONTROL_BACK,
    CHEAT_CONTROL_REVEAL,
    CHEAT_CONTROL_PAGE_PREV,
    CHEAT_CONTROL_PAGE_NEXT
} CheatsheetControl;

void CheatsheetInit(void);

void CheatsheetOpen(CheatsheetState *state, int levelIndex);

int CheatsheetStepLevel(CheatsheetState *state, int delta);

void CheatsheetToggleReveal(CheatsheetState *state);

int CheatsheetStepPage(CheatsheetState *state, const CheatsheetLayout *layout,
                       int delta);

const CheatRoute *CheatsheetRoute(int levelIndex);
int CheatsheetPageCount(const CheatsheetLayout *layout, int levelIndex);

CheatsheetLayout CheatsheetGetLayout(Assets *asset, float screenWidth,
                                    float screenHeight);

CheatsheetControl CheatsheetHitTest(const CheatsheetLayout *layout,
                                    const CheatsheetState *state,
                                    Vector2 mouse);
void CheatsheetDraw(Assets *asset, const CheatsheetLayout *layout,
                    const CheatsheetState *state, CheatsheetControl hovered);

#endif
