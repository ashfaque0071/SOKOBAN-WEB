#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include "raylib.h"
#include "assets.h"

#include "score.h"
#include "ui.h"

#define HIGHSCORE_ROWS 7

#define HIGHSCORE_COLUMNS 6

typedef struct HighScoresState {
    int page;
} HighScoresState;

typedef struct HighScoresLayout {
    Rectangle screen;
    Rectangle panel;
    Rectangle title;
    Rectangle header;
    Rectangle row[HIGHSCORE_ROWS];
    Rectangle total;
    Rectangle pageMark;
    Rectangle prevTab;
    Rectangle nextTab;
    Rectangle backButton;

    float columnCenter[HIGHSCORE_COLUMNS];
    float columnWidth[HIGHSCORE_COLUMNS];

    float addedRuleX[HIGHSCORE_COLUMNS - 3];
    float ruleTop;
    float ruleBottom;
} HighScoresLayout;

typedef enum HighScoresControl {
    HIGHSCORE_CONTROL_NONE = -1,
    HIGHSCORE_CONTROL_PREV,
    HIGHSCORE_CONTROL_NEXT,
    HIGHSCORE_CONTROL_BACK
} HighScoresControl;

void HighScoresOpen(HighScoresState *state, int levelIndex);
int HighScoresPageCount(void);

int HighScoresStepPage(HighScoresState *state, int delta);

HighScoresLayout HighScoresGetLayout(Assets *asset, float screenWidth,
                                     float screenHeight);

HighScoresControl HighScoresHitTest(const HighScoresLayout *layout,
                                    const HighScoresState *state,
                                    Vector2 mouse);
void HighScoresDraw(Assets *asset, const HighScoresLayout *layout,
                    const HighScoresState *state, HighScoresControl hovered);

#endif
