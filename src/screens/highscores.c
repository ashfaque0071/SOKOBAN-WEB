#include "highscores.h"
#include "board.h"
#include "colors.h"

static const float ROW_RULE[HIGHSCORE_ROWS + 1] = {
    0.2736f, 0.3512f, 0.4267f, 0.5011f,
    0.5734f, 0.6477f, 0.7211f, 0.7965f
};

#define HEADER_TOP 0.1960f

#define RULE_LEFT 0.2318f
#define RULE_RIGHT 0.7002f
#define PAPER_LEFT 0.0680f
#define PAPER_RIGHT 0.9500f

#define TITLE_TOP 0.0560f
#define TITLE_BOTTOM 0.1730f

#define TOTAL_TOP 0.8080f
#define TOTAL_BOTTOM 0.8800f

int HighScoresPageCount(void)
{
    return (LEVEL_COUNT + HIGHSCORE_ROWS - 1) / HIGHSCORE_ROWS;
}

void HighScoresOpen(HighScoresState *state, int levelIndex)
{
    if (levelIndex < 0)
        levelIndex = 0;
    if (levelIndex >= LEVEL_COUNT)
        levelIndex = LEVEL_COUNT - 1;
    state->page = levelIndex / HIGHSCORE_ROWS;
}

int HighScoresStepPage(HighScoresState *state, int delta)
{
    int target = state->page + delta;
    if (target < 0 || target >= HighScoresPageCount())
        return 0;
    state->page = target;
    return 1;
}

HighScoresLayout HighScoresGetLayout(Assets *asset, float screenWidth,
                                     float screenHeight)
{
    HighScoresLayout layout = {0};
    const float W = screenWidth;
    const float H = screenHeight;

    layout.screen = (Rectangle){0, 0, W, H};
    layout.panel = layout.screen;

    layout.title = (Rectangle){
        W * 0.180f, H * TITLE_TOP,
        W * 0.640f, H * (TITLE_BOTTOM - TITLE_TOP)
    };
    layout.header = (Rectangle){
        W * PAPER_LEFT, H * HEADER_TOP,
        W * (PAPER_RIGHT - PAPER_LEFT), H * (ROW_RULE[0] - HEADER_TOP)
    };
    for (int i = 0; i < HIGHSCORE_ROWS; i++)
        layout.row[i] = (Rectangle){
            W * PAPER_LEFT, H * ROW_RULE[i],
            W * (PAPER_RIGHT - PAPER_LEFT), H * (ROW_RULE[i + 1] - ROW_RULE[i])
        };
    layout.total = (Rectangle){
        W * PAPER_LEFT, H * TOTAL_TOP,
        W * (PAPER_RIGHT - PAPER_LEFT), H * (TOTAL_BOTTOM - TOTAL_TOP)
    };

    layout.pageMark = (Rectangle){
        W * RULE_RIGHT, H * TOTAL_TOP,
        W * (PAPER_RIGHT - RULE_RIGHT), H * (TOTAL_BOTTOM - TOTAL_TOP)
    };

    float numberSpan = (RULE_RIGHT - RULE_LEFT) / 4.0f;
    float edge[HIGHSCORE_COLUMNS + 1] = {
        PAPER_LEFT,
        RULE_LEFT,
        RULE_LEFT + numberSpan,
        RULE_LEFT + numberSpan * 2.0f,
        RULE_LEFT + numberSpan * 3.0f,
        RULE_RIGHT,
        PAPER_RIGHT
    };
    for (int i = 0; i < HIGHSCORE_COLUMNS; i++)
    {
        layout.columnCenter[i] = W * (edge[i] + edge[i + 1]) / 2.0f;
        layout.columnWidth[i] = W * (edge[i + 1] - edge[i]);
    }
    for (int i = 0; i < HIGHSCORE_COLUMNS - 3; i++)
        layout.addedRuleX[i] = W * edge[i + 2];
    layout.ruleTop = H * (HEADER_TOP + 0.010f);
    layout.ruleBottom = H * ROW_RULE[HIGHSCORE_ROWS];

    float controlY = H * 0.9450f;
    float tabWidth = W * 0.135f;
    float tabHeight = tabWidth * asset->texCheatTab.height /
                      asset->texCheatTab.width;
    layout.prevTab = (Rectangle){
        W * 0.182f, controlY - tabHeight / 2.0f, tabWidth, tabHeight
    };
    layout.nextTab = (Rectangle){
        W - W * 0.182f - tabWidth, controlY - tabHeight / 2.0f,
        tabWidth, tabHeight
    };

    float buttonWidth = W * 0.170f;
    float buttonHeight = H * 0.068f;
    layout.backButton = (Rectangle){
        (W - buttonWidth) / 2.0f, controlY - buttonHeight / 2.0f,
        buttonWidth, buttonHeight
    };
    return layout;
}

HighScoresControl HighScoresHitTest(const HighScoresLayout *layout,
                                    const HighScoresState *state,
                                    Vector2 mouse)
{
    if (state->page > 0 &&
        CheckCollisionPointRec(mouse, layout->prevTab))
        return HIGHSCORE_CONTROL_PREV;
    if (state->page < HighScoresPageCount() - 1 &&
        CheckCollisionPointRec(mouse, layout->nextTab))
        return HIGHSCORE_CONTROL_NEXT;
    if (CheckCollisionPointRec(mouse, layout->backButton))
        return HIGHSCORE_CONTROL_BACK;
    return HIGHSCORE_CONTROL_NONE;
}

static const Color SHEET_INK = {38, 33, 28, 255};
static const Color SHEET_RULE = {108, 100, 92, 255};
#define SHEET_TRACKING 0.055f

static void DrawSheetTextCentered(Font font, const char *text, float centerX,
                                  float centerY, float size, Color color)
{
    float spacing = size * SHEET_TRACKING;
    Vector2 measured = MeasureTextEx(font, text, size, spacing);
    DrawTextEx(font, text,
               (Vector2){centerX - measured.x / 2.0f,
                         centerY - measured.y / 2.0f},
               size, spacing, color);
}

static void DrawNoRecordDash(float centerX, float centerY, float size,
                             Color color)
{
    float width = size * 0.52f;
    DrawLineEx((Vector2){centerX - width / 2.0f, centerY},
               (Vector2){centerX + width / 2.0f, centerY},
               size * 0.085f, color);
}

static void DrawWholeTexture(Texture2D texture, Rectangle destination,
                             Color tint)
{
    Rectangle source = {0, 0, (float)texture.width, (float)texture.height};
    DrawTexturePro(texture, source, destination, (Vector2){0, 0}, 0, tint);
}

static void DrawPageTab(Assets *asset, Rectangle tab, const char *label,
                        bool pointingLeft, bool hovered, bool enabled)
{
    Color tint = enabled ? WHITE : Fade(WHITE, 0.62f);
    DrawWholeTexture(asset->texCheatTab, tab, tint);

    if (hovered && enabled)
    {
        BeginBlendMode(BLEND_ADDITIVE);
        DrawWholeTexture(asset->texCheatTab, tab, COL_HOVER_LIFT);
        EndBlendMode();
    }

    float arrowSize = tab.height * 0.34f;
    float inset = tab.width * 0.14f;
    Rectangle box = {
        pointingLeft ? tab.x + inset : tab.x + tab.width - inset - arrowSize,
        tab.y + tab.height * 0.50f - arrowSize / 2.0f,
        arrowSize, arrowSize
    };
    Rectangle source = {
        0, 0, (float)asset->texCheatArrow.width,
        (float)asset->texCheatArrow.height
    };
    Rectangle destination = {
        box.x + box.width / 2.0f, box.y + box.height / 2.0f,
        box.width, box.height
    };
    DrawTexturePro(asset->texCheatArrow, source, destination,
                   (Vector2){box.width / 2.0f, box.height / 2.0f},
                   pointingLeft ? 270.0f : 90.0f,
                   enabled ? SHEET_INK : Fade(SHEET_INK, 0.40f));

    float labelCenterX = pointingLeft
        ? tab.x + tab.width * 0.62f
        : tab.x + tab.width * 0.38f;
    DrawSheetTextCentered(asset->fontCondensed, label, labelCenterX,
                          tab.y + tab.height * 0.50f, tab.height * 0.26f,
                          enabled ? SHEET_INK : Fade(SHEET_INK, 0.40f));

}

static void DrawRatingStars(Assets *asset, Rectangle cell, int stars,
                            float opacity)
{
    float starSize = cell.height * 0.62f;
    float gap = starSize * 0.22f;
    float totalWidth = starSize * 3.0f + gap * 2.0f;
    float x = cell.x + (cell.width - totalWidth) / 2.0f;
    float y = cell.y + (cell.height - starSize) / 2.0f;

    for (int i = 0; i < 3; i++)
    {
        Texture2D star = i < stars
            ? asset->texLevelsStarGold
            : asset->texLevelsStarGrey;
        Rectangle box = {x + i * (starSize + gap), y, starSize, starSize};
        DrawWholeTexture(star, FitInside(star, box), Fade(WHITE, opacity));
    }
}

static void DrawCaseRow(Assets *asset, const HighScoresLayout *layout,
                        int row, int level)
{
    Rectangle band = layout->row[row];
    float centerY = band.y + band.height / 2.0f;
    float textSize = band.height * 0.46f;

    bool unlocked = LevelUnlocked(level);
    bool cleared = LevelCleared(level);
    Color ink = unlocked ? SHEET_INK : Fade(SHEET_INK, 0.45f);

    DrawSheetTextCentered(asset->fontCondensed, TextFormat("%i", level + 1),
                          layout->columnCenter[0], centerY, textSize, ink);

    int reported[4] = {
        unlocked ? LevelMinimumPushes(level) : -1,
        cleared ? LevelBestPushes(level) : -1,
        cleared ? LevelBestMoves(level) : -1,
        cleared ? LevelBestScore(level) : -1
    };
    for (int i = 1; i < 4; i++)
        if (reported[i] == 0)
            reported[i] = -1;
    for (int i = 0; i < 4; i++)
    {
        if (reported[i] < 0)
            DrawNoRecordDash(layout->columnCenter[i + 1], centerY, textSize,
                             ink);
        else
            DrawSheetTextCentered(asset->fontCondensed,
                                  TextFormat("%i", reported[i]),
                                  layout->columnCenter[i + 1], centerY,
                                  textSize, ink);
    }

    Rectangle rating = {
        layout->columnCenter[5] - layout->columnWidth[5] / 2.0f,
        band.y, layout->columnWidth[5], band.height
    };
    bool inProgress = unlocked && !cleared;
    if (inProgress)
        rating.width *= 0.62f;

    DrawRatingStars(asset, rating, cleared ? LevelBestStars(level) : 0,
                    unlocked ? 1.0f : 0.45f);

    if (inProgress)
    {
        float stampHeight = band.height * 0.52f;
        float stampWidth = stampHeight * asset->texLevelsStampProgress.width /
                           asset->texLevelsStampProgress.height;
        DrawWholeTexture(asset->texLevelsStampProgress, (Rectangle){
            rating.x + rating.width + layout->columnWidth[4] * 0.030f,
            centerY - stampHeight / 2.0f, stampWidth, stampHeight
        }, WHITE);
    }
}

void HighScoresDraw(Assets *asset, const HighScoresLayout *layout,
                    const HighScoresState *state, HighScoresControl hovered)
{
    DrawWholeTexture(asset->texLevelsBackground, layout->screen, WHITE);
    DrawWholeTexture(asset->texHighScoresPanel, layout->panel, WHITE);

    float titleCenterX = layout->title.x + layout->title.width / 2.0f;
    float titleSize = layout->title.height * 0.62f;
    DrawSheetTextCentered(asset->fontCondensed, "CASE PERFORMANCE",
                          titleCenterX,
                          layout->title.y + layout->title.height * 0.50f,
                          titleSize, SHEET_INK);

    for (int i = 0; i < HIGHSCORE_COLUMNS - 3; i++)
        DrawLineEx((Vector2){layout->addedRuleX[i], layout->ruleTop},
                   (Vector2){layout->addedRuleX[i], layout->ruleBottom},
                   1.6f, Fade(SHEET_RULE, 0.85f));

    static const char *HEADING[HIGHSCORE_COLUMNS] = {
        "LEVEL", "MIN PUSHES", "BEST PUSHES", "BEST MOVES", "SCORE", "RATING"
    };
    float headingSize = layout->header.height * 0.40f;
    float headingY = layout->header.y + layout->header.height * 0.62f;
    for (int i = 0; i < HIGHSCORE_COLUMNS; i++)
        DrawSheetTextCentered(asset->fontCondensed, HEADING[i],
                              layout->columnCenter[i], headingY, headingSize,
                              SHEET_INK);

    int firstLevel = state->page * HIGHSCORE_ROWS;
    for (int row = 0; row < HIGHSCORE_ROWS; row++)
    {
        int level = firstLevel + row;
        if (level >= LEVEL_COUNT)
            break;
        DrawCaseRow(asset, layout, row, level);
    }

    int earned = 0;
    for (int i = 0; i < LEVEL_COUNT; i++)
        earned += LevelBestStars(i);

    float totalCenterX = layout->total.x + layout->total.width / 2.0f;
    float totalSize = layout->total.height * 0.66f;
    DrawSheetTextCentered(asset->fontCondensed,
                          TextFormat("TOTAL: %i / %i STARS", earned,
                                     LEVEL_COUNT * 3),
                          totalCenterX,
                          layout->total.y + layout->total.height * 0.50f,
                          totalSize, SHEET_INK);

    int pageCount = HighScoresPageCount();
    DrawSheetTextCentered(asset->fontCondensed,
                          TextFormat("PAGE %i / %i", state->page + 1,
                                     pageCount),
                          layout->pageMark.x + layout->pageMark.width / 2.0f,
                          layout->pageMark.y + layout->pageMark.height * 0.42f,
                          layout->pageMark.height * 0.34f,
                          Fade(SHEET_INK, 0.75f));

    DrawPageTab(asset, layout->prevTab, "PREV", true,
                hovered == HIGHSCORE_CONTROL_PREV, state->page > 0);
    DrawPageTab(asset, layout->nextTab, "NEXT", false,
                hovered == HIGHSCORE_CONTROL_NEXT, state->page < pageCount - 1);

    Texture2D plate = hovered == HIGHSCORE_CONTROL_BACK
        ? asset->texPauseButtonHover
        : asset->texPauseButtonNormal;
    DrawPauseButtonBase(plate, layout->backButton, WHITE);
    DrawSheetTextCentered(asset->fontCondensed, "BACK",
                          layout->backButton.x + layout->backButton.width / 2.0f,
                          layout->backButton.y + layout->backButton.height * 0.48f,
                          layout->backButton.height * 0.38f, SHEET_INK);
}
