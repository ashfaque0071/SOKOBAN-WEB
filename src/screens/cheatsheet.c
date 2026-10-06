#include <stddef.h>

#include "raylib.h"
#include "cheatsheet.h"
#include "colors.h"

static const struct {
    const char *route;
    int moves;
    int pushes;
} ROUTE_DATA[LEVEL_COUNT] = {

    {
        "U L U R3 D2 R D R D L3 ",
        15, 7
    },

    {
        "D L2 U D L U L U2 R D R D2 L U ",
        17, 5
    },

    {
        "U L U2 R D L D3 R2 U R U2 L2 U L D ",
        21, 5
    },

    {
        "L U L D U2 R3 D L R2 D2 L U L2 D R L D L2 U R ",
        27, 6
    },

    {
        "U R2 D U L2 D R D R L U2 R D R D2 L U R U L D L U R U L ",
        30, 10
    },

    {
        "R D U L2 D R D2 R2 U L D L U2 L U2 R2 D2 L D R U3 L D2 ",
        33, 9
    },

    {
        "R U2 L3 U L D R4 D2 L U R U L3 D2 L3 U2 R2 D R D L U3 R D2 ",
        41, 13
    },

    {
        "D R3 U2 L D L D L U3 L U R2 D L D2 R3 D L2 D L U4 R U L D3 R2 "
        "U R U2 L ",
        47, 15
    },

    {
        "U2 L U L2 D R D R L U2 R D R D2 R D2 L2 U R U3 L2 D R U R D2 "
        "R3 D2 L2 U D L2 U R ",
        49, 11
    },

    {
        "U R D3 R3 U2 L4 R D2 L D2 R U3 D R3 U2 L2 U L2 D R3 L U L4 D "
        "R4 ",
        50, 16
    },

    {
        "R U4 L3 D2 R2 D R U2 D L3 U3 R4 D L D5 L U2 R U3 L U L2 D3 R2 "
        "D R U2 D L3 U3 R2 D L U R3 D L ",
        71, 13
    },

    {
        "U L3 D4 R4 U L D L3 U4 R3 D3 R D L4 U L2 D2 R U L U R D R4 U2 "
        "L D R D L3 U L2 D2 R U R3 U2 R U2 L3 D2 ",
        78, 16
    },

    {
        "D2 L2 U L U L2 D R2 D R3 U2 L4 D L U2 L U R3 L2 D3 R2 D R2 U R "
        "U L4 D L U2 L U R2 L D3 R2 D R U R U L3 D L U2 L U R D L2 D R2 "
        "D R2 U L D L U2 ",
        92, 34
    },

    {
        "D R3 U R2 U2 L2 D L D U R U R2 D2 L2 D L2 D L2 U2 R D L D R U3 "
        "R2 D R3 U2 L2 D U R2 D2 L2 D L2 R2 U2 L3 D2 U2 R U R D2 R3 U2 "
        "L U L D2 U R2 D2 L2 D L2 R2 U2 L D R D L ",
        105, 25
    },

    {
        "U3 L6 R2 D2 L2 U L2 D L2 U R3 U2 L U2 R D3 R7 U2 R2 D2 L R U2 "
        "L2 D2 L4 D2 L2 U L2 D L2 U R3 U R7 D2 R2 U2 L R D2 L2 U2 D2 R "
        "D2 L U2 R2 U2 L6 D2 L2 U L U R6 ",
        126, 48
    },

    {
        "L2 D L3 U2 R2 D U L2 D2 L2 U R D R3 D R U L U R4 U R2 D2 L U R "
        "U L2 D L4 U L U2 R D2 L2 D L D R3 D R U L7 U R3 L2 U2 R3 D R2 "
        "U2 L D L D2 L D R3 U R4 U R2 D2 L U L5 D2 R U L U R3 L U R D "
        "R2 D R U ",
        135, 43
    },

    {
        "D L R U2 L D L D2 L D2 R2 U L U3 R2 D L U L D2 L3 D2 R2 U D R2 U L ",
        41, 10
    },

    {
        "D2 L3 U2 R4 L D2 R D2 L U3 D L3 U2 R2 U R2 D L3 R U R4 D L4 ",
        47, 15
    },

    {
        "D L U2 D R3 U3 L4 D R D5 R U2 L U3 R U R2 D3 L2 D L U2 D R3 U3 L2 "
        "D R U L3 D R ",
        59, 12
    },

    {
        "D R3 U4 L3 D3 L D R4 U R2 D2 L U R U L D L4 U2 R D L D R3 U R2 D2 "
        "L U L3 U2 L U2 R3 D2 ",
        64, 15
    },

    {
        "U R2 D L2 D L3 U2 R4 D R U2 R U L3 R2 D3 L2 D L2 U L U R4 D R U2 R "
        "U L2 R D3 L2 D L U L U R3 D R U2 R U L D R2 D L2 D L2 U R D R U2 ",
        86, 33
    },

    {
        "L2 U2 R2 D R D U L U L2 D2 R2 D R2 D R2 U2 L D R D L U3 L2 D L3 U2 "
        "R2 D U L2 D2 R2 D R2 L2 U2 R3 D2 U2 L U L D2 L3 U2 R U R D2 U L2 "
        "D2 R2 D R2 L2 U2 R D L D R ",
        100, 24
    },

    {
        "R L2 D2 R2 U R2 D R2 U L3 U2 R U2 L D3 L7 U2 L2 D2 R L U2 R2 D2 R4 "
        "D2 R2 U R2 D R2 U L3 U L7 D2 L2 U2 R L D2 R2 U2 D2 L D2 R U2 L2 U2 "
        "R6 D2 R2 U R U L6 ",
        118, 47
    },

    {
        "U L2 D U R2 D2 R2 U L D L3 D L U R U L4 U L2 D2 R U L U R2 D R4 U "
        "R U2 L D2 R2 D R D L3 D L U R7 U L3 R2 U2 L3 D L2 U2 R D R D2 R D "
        "L3 U L4 U L2 D2 R U R5 D2 L U R U L3 R U L D L2 D L U ",
        128, 42
    },

    {
        "L D2 R U R U2 R U2 L2 D R D3 L2 U R D R U2 R3 U2 L2 D U L2 D R ",
        39, 9
    },

    {
        "U R3 D2 L4 R U2 L U2 R D3 U R3 D2 L2 D L2 U R3 L D L4 U R4 ",
        46, 14
    },

    {
        "R D2 U L3 D3 R4 U L U5 L D2 R D3 L D L2 U3 R2 U R D2 U L3 D3 R2 U "
        "L D R3 U L ",
        58, 11
    },

    {
        "U R U L4 D L2 U2 R D L D R U R4 D2 L U R U L3 D L2 U2 R D R3 D2 R "
        "D2 L3 U2 ",
        51, 14
    },

    {
        "L U R2 U R3 D2 L4 U L D2 L D R3 L2 U3 R2 U R2 D R D L4 U L D2 L D "
        "R2 L U3 R2 U R D R D L3 U L D2 L D R U L2 U R2 U R2 D L U L D2 ",
        84, 32
    },

    {
        "L U D R D R2 U2 L2 U L2 U L2 D2 R U L U R D3 R2 U R3 D2 L2 U D R2 "
        "U2 L2 U L2 R2 D2 L3 U2 D2 R D R U2 R3 D2 L D L U2 D R2 U2 L2 U L2 "
        "R2 D2 L U R U L ",
        93, 23
    },

    {
        "R2 U2 L2 D L2 U L2 D R3 D2 L D2 R U3 R7 D2 R2 U2 L R D2 L2 U2 L4 "
        "U2 L2 D L2 U L2 D R3 D R7 U2 R2 D2 L R U2 L2 D2 U2 R U2 L D2 R2 D2 "
        "L6 U2 L2 D L D R6 ",
        117, 46
    },

    {
        "R2 U D L2 U2 L2 D R U R3 U R D L D R4 D R2 U2 L D R D L2 U L4 D L "
        "D2 R U2 L2 U L U R3 U R D L7 D R3 L2 D2 R3 U R2 D2 L U L U2 L U R3 "
        "D R4 D R2 U2 L D L5 U2 R D L D R3 L D R U R2 U R D ",
        127, 41
    },

    {
        "L D2 L D2 R2 U L U3 R2 D L U L D2 L3 D2 R2 U D R2 U L ",
        34, 8
    },

    {
        "L3 U2 R4 L D2 R D2 L U3 D L3 U2 R2 U R2 D L3 R U R4 D L4 ",
        45, 13
    },

    {
        "U D R3 U3 L4 D R D5 R U2 L U3 R U R2 D3 L2 D L U2 D R3 U3 L2 D R U "
        "L3 D R ",
        56, 10
    },

    {
        "L D R4 U R2 D2 L U R U L D L4 U2 R D L D R3 U R2 D2 L U L3 U2 L U2 "
        "R3 D2 ",
        50, 13
    },

    {
        "D L2 D L3 U2 R4 D R U2 R U L3 R2 D3 L2 D L2 U L U R4 D R U2 R U L2 "
        "R D3 L2 D L U L U R3 D R U2 R U L D R2 D L2 D L2 U R D R U2 ",
        83, 31
    },

    {
        "U L U L2 D2 R2 D R2 D R2 U2 L D R D L U3 L2 D L3 U2 R2 D U L2 D2 "
        "R2 D R2 L2 U2 R3 D2 U2 L U L D2 L3 U2 R U R D2 U L2 D2 R2 D R2 L2 "
        "U2 R D L D R ",
        91, 22
    },

    {
        "R2 D R2 U L3 U2 R U2 L D3 L7 U2 L2 D2 R L U2 R2 D2 R4 D2 R2 U R2 D "
        "R2 U L3 U L7 D2 L2 U2 R L D2 R2 U2 D2 L D2 R U2 L2 U2 R6 D2 R2 U R "
        "U L6 ",
        110, 45
    },

    {
        "U R2 D2 R2 U L D L3 D L U R U L4 U L2 D2 R U L U R2 D R4 U R U2 L "
        "D2 R2 D R D L3 D L U R7 U L3 R2 U2 L3 D L2 U2 R D R D2 R D L3 U L4 "
        "U L2 D2 R U R5 D2 L U R U L3 R U L D L2 D L U ",
        124, 40
    },

    {
        "U R U2 L2 D R D3 L2 U R D R U2 R3 U2 L2 D U L2 D R ",
        32, 7
    },

    {
        "L2 R U2 L U2 R D3 U R3 D2 L2 D L2 U R3 L D L4 U R4 ",
        38, 12
    },

    {
        "U L3 D3 R4 U L U5 L D2 R D3 L D L2 U3 R2 U R D2 U L3 D3 R2 U L D "
        "R3 U L ",
        55, 9
    },

    {
        "L3 D L2 U2 R D L D R U R4 D2 L U R U L3 D L2 U2 R D R3 D2 R D2 L3 "
        "U2 ",
        47, 12
    },

    {
        "R U R3 D2 L4 U L D2 L D R3 L2 U3 R2 U R2 D R D L4 U L D2 L D R2 L "
        "U3 R2 U R D R D L3 U L D2 L D R U L2 U R2 U R2 D L U L D2 ",
        81, 30
    },

    {
        "U L2 U L2 D2 R U L U R D3 R2 U R3 D2 L2 U D R2 U2 L2 U L2 R2 D2 L3 "
        "U2 D2 R D R U2 R3 D2 L D L U2 D R2 U2 L2 U L2 R2 D2 L U R U L ",
        82, 21
    },

    {
        "L U L2 D R3 D2 L D2 R U3 R7 D2 R2 U2 L R D2 L2 U2 L4 U2 L2 D L2 U "
        "L2 D R3 D R7 U2 R2 D2 L R U2 L2 D2 U2 R U2 L D2 R2 D2 L6 U2 L2 D L "
        "D R6 ",
        109, 44
    },

    {
        "U R3 U R D L D R4 D R2 U2 L D R D L2 U L4 D L D2 R U2 L2 U L U R3 "
        "U R D L7 D R3 L2 D2 R3 U R2 D2 L U L U2 L U R3 D R4 D R2 U2 L D L5 "
        "U2 R D L D R3 L D R U R2 U R D ",
        115, 39
    }
};

static CheatRoute routes[LEVEL_COUNT];

static void ParseRoute(const char *text, CheatRoute *route)
{
    route->stepCount = 0;
    for (int i = 0; text[i] != '\0'; i++)
    {
        int direction;
        switch (text[i])
        {
            case 'U': direction = CHEAT_UP; break;
            case 'D': direction = CHEAT_DOWN; break;
            case 'L': direction = CHEAT_LEFT; break;
            case 'R': direction = CHEAT_RIGHT; break;
            default: continue;
        }

        int repeat = 0;
        while (text[i + 1] >= '0' && text[i + 1] <= '9')
        {
            repeat = repeat * 10 + (text[i + 1] - '0');
            i++;
        }
        if (repeat < 1)
            repeat = 1;

        if (route->stepCount >= CHEAT_MAX_STEPS)
            return;
        route->step[route->stepCount].direction = (unsigned char)direction;
        route->step[route->stepCount].repeat = (unsigned short)repeat;
        route->stepCount++;
    }
}

void CheatsheetInit(void)
{
    for (int i = 0; i < LEVEL_COUNT; i++)
    {
        ParseRoute(ROUTE_DATA[i].route, &routes[i]);
        routes[i].optimalMoves = ROUTE_DATA[i].moves;
        routes[i].optimalPushes = ROUTE_DATA[i].pushes;
    }
}

const CheatRoute *CheatsheetRoute(int levelIndex)
{
    if (levelIndex < 0)
        levelIndex = 0;
    if (levelIndex >= LEVEL_COUNT)
        levelIndex = LEVEL_COUNT - 1;
    return &routes[levelIndex];
}

#define PAPER_X 0.088f
#define PAPER_Y 0.080f
#define PAPER_W 0.846f
#define PAPER_H 0.795f

#define ROUTE_ROWS 4
#define FOOTER_RATIO 0.150f

CheatsheetLayout CheatsheetGetLayout(Assets *asset, float screenWidth,
                                    float screenHeight)
{
    CheatsheetLayout layout = {0};
    layout.screen = (Rectangle){0, 0, screenWidth, screenHeight};
    const float SCREEN_W = screenWidth;
    const float SCREEN_H = screenHeight;

    float titleWidth = SCREEN_W * 0.320f;
    layout.title = (Rectangle){
        (SCREEN_W - titleWidth) / 2.0f, SCREEN_H * 0.030f, titleWidth,
        SCREEN_H * 0.098f
    };

    float plateWidth = SCREEN_W * 0.350f;
    layout.levelPlate = (Rectangle){
        (SCREEN_W - plateWidth) / 2.0f, SCREEN_H * 0.160f, plateWidth,
        SCREEN_H * 0.098f
    };

    float noteWidth = SCREEN_W * 0.228f;
    layout.note = (Rectangle){
        SCREEN_W * 0.712f, SCREEN_H * 0.016f, noteWidth,
        noteWidth * asset->texCheatNote.height / asset->texCheatNote.width
    };

    layout.panel = (Rectangle){
        SCREEN_W * 0.196f, SCREEN_H * 0.250f,
        SCREEN_W * 0.608f, SCREEN_H * 0.560f
    };
    layout.paper = (Rectangle){
        layout.panel.x + layout.panel.width * PAPER_X,
        layout.panel.y + layout.panel.height * PAPER_Y,
        layout.panel.width * PAPER_W,
        layout.panel.height * PAPER_H
    };

    float tabWidth = SCREEN_W * 0.145f;
    float tabHeight = tabWidth * asset->texCheatTab.height /
                      asset->texCheatTab.width;
    layout.prevTab = (Rectangle){
        SCREEN_W * 0.108f, SCREEN_H * 0.773f, tabWidth, tabHeight
    };
    layout.nextTab = (Rectangle){
        SCREEN_W - SCREEN_W * 0.108f - tabWidth, SCREEN_H * 0.773f,
        tabWidth, tabHeight
    };

    float buttonWidth = SCREEN_W * 0.228f;
    float buttonHeight = SCREEN_H * 0.077f;
    float buttonGap = SCREEN_W * 0.030f;
    float buttonY = SCREEN_H * 0.876f;
    float buttonX = (SCREEN_W - (buttonWidth * 2.0f + buttonGap)) / 2.0f;
    layout.backButton = (Rectangle){
        buttonX, buttonY, buttonWidth, buttonHeight
    };
    layout.revealButton = (Rectangle){
        buttonX + buttonWidth + buttonGap, buttonY, buttonWidth, buttonHeight
    };

    float footerHeight = layout.paper.height * FOOTER_RATIO;
    float arrowSize = footerHeight * 0.62f;
    float arrowInset = layout.paper.width * 0.075f;
    float arrowY = layout.paper.y + layout.paper.height - footerHeight +
                   (footerHeight - arrowSize) / 2.0f;
    layout.pagePrev = (Rectangle){
        layout.paper.x + arrowInset, arrowY, arrowSize, arrowSize
    };
    layout.pageNext = (Rectangle){
        layout.paper.x + layout.paper.width - arrowInset - arrowSize, arrowY,
        arrowSize, arrowSize
    };
    return layout;
}

typedef struct RouteGrid {
    Rectangle rows;
    float rowHeight;
    float numberWidth;
    float unitWidth;
    int unitsPerRow;
    int unitsPerPage;
} RouteGrid;

static RouteGrid GridOf(const CheatsheetLayout *layout)
{
    RouteGrid grid;
    float footerHeight = layout->paper.height * FOOTER_RATIO;
    float padX = layout->paper.width * 0.045f;
    float padY = layout->paper.height * 0.045f;

    grid.rows = (Rectangle){
        layout->paper.x + padX,
        layout->paper.y + padY,
        layout->paper.width - padX * 2.0f,
        layout->paper.height - footerHeight - padY
    };
    grid.rowHeight = grid.rows.height / ROUTE_ROWS;
    grid.numberWidth = grid.rows.width * 0.080f;

    float minimumUnit = grid.rows.width * 0.118f;
    grid.unitsPerRow = (int)((grid.rows.width - grid.numberWidth) /
                             minimumUnit);
    if (grid.unitsPerRow < 1)
        grid.unitsPerRow = 1;
    grid.unitWidth = (grid.rows.width - grid.numberWidth) /
                     (float)grid.unitsPerRow;
    grid.unitsPerPage = grid.unitsPerRow * ROUTE_ROWS;
    return grid;
}

int CheatsheetPageCount(const CheatsheetLayout *layout, int levelIndex)
{
    const CheatRoute *route = CheatsheetRoute(levelIndex);
    RouteGrid grid = GridOf(layout);
    int pages = (route->stepCount + grid.unitsPerPage - 1) / grid.unitsPerPage;
    return pages < 1 ? 1 : pages;
}

void CheatsheetOpen(CheatsheetState *state, int levelIndex)
{
    if (levelIndex < 0)
        levelIndex = 0;
    if (levelIndex >= LEVEL_COUNT)
        levelIndex = LEVEL_COUNT - 1;
    state->level = levelIndex;
    state->revealed = false;
    state->page = 0;
}

int CheatsheetStepLevel(CheatsheetState *state, int delta)
{
    int target = state->level + delta;
    if (target < 0 || target >= LEVEL_COUNT)
        return 0;
    CheatsheetOpen(state, target);
    return 1;
}

void CheatsheetToggleReveal(CheatsheetState *state)
{
    if (!LevelUnlocked(state->level))
        return;
    state->revealed = !state->revealed;
    state->page = 0;
}

int CheatsheetStepPage(CheatsheetState *state, const CheatsheetLayout *layout,
                       int delta)
{
    if (!state->revealed || !LevelUnlocked(state->level))
        return 0;

    int pages = CheatsheetPageCount(layout, state->level);
    int target = state->page + delta;
    if (target < 0)
        target = 0;
    if (target > pages - 1)
        target = pages - 1;
    if (target == state->page)
        return 0;
    state->page = target;
    return 1;
}

CheatsheetControl CheatsheetHitTest(const CheatsheetLayout *layout,
                                    const CheatsheetState *state,
                                    Vector2 mouse)
{
    bool unlocked = LevelUnlocked(state->level);
    int pages = CheatsheetPageCount(layout, state->level);
    bool paging = state->revealed && unlocked && pages > 1;

    if (state->level > 0 &&
        CheckCollisionPointRec(mouse, layout->prevTab))
        return CHEAT_CONTROL_PREV;
    if (state->level < LEVEL_COUNT - 1 &&
        CheckCollisionPointRec(mouse, layout->nextTab))
        return CHEAT_CONTROL_NEXT;
    if (CheckCollisionPointRec(mouse, layout->backButton))
        return CHEAT_CONTROL_BACK;
    if (unlocked && CheckCollisionPointRec(mouse, layout->revealButton))
        return CHEAT_CONTROL_REVEAL;
    if (paging && state->page > 0 &&
        CheckCollisionPointRec(mouse, layout->pagePrev))
        return CHEAT_CONTROL_PAGE_PREV;
    if (paging && state->page < pages - 1 &&
        CheckCollisionPointRec(mouse, layout->pageNext))
        return CHEAT_CONTROL_PAGE_NEXT;
    return CHEAT_CONTROL_NONE;
}

static const Color CHEAT_INK = {32, 28, 24, 255};
static const Color CHEAT_FAINT = {96, 86, 76, 255};

#define CHEAT_TRACKING 0.045f

static void DrawCheatTextCentered(Font font, const char *text, float centerX,
                                  float centerY, float size, Color color)
{
    float spacing = size * CHEAT_TRACKING;
    Vector2 measured = MeasureTextEx(font, text, size, spacing);
    DrawTextEx(font, text,
               (Vector2){centerX - measured.x / 2.0f,
                         centerY - measured.y / 2.0f},
               size, spacing, color);
}

static void DrawWholeTinted(Texture2D texture, Rectangle destination,
                            Color tint)
{
    Rectangle source = {0, 0, (float)texture.width, (float)texture.height};
    DrawTexturePro(texture, source, destination, (Vector2){0, 0}, 0, tint);
}

static void DrawDirectionArrow(Texture2D arrow, Rectangle box, int direction,
                               Color tint)
{
    float rotation = 0.0f;
    if (direction == CHEAT_RIGHT)
        rotation = 90.0f;
    else if (direction == CHEAT_DOWN)
        rotation = 180.0f;
    else if (direction == CHEAT_LEFT)
        rotation = 270.0f;

    Rectangle source = {0, 0, (float)arrow.width, (float)arrow.height};
    Rectangle destination = {
        box.x + box.width / 2.0f, box.y + box.height / 2.0f,
        box.width, box.height
    };
    DrawTexturePro(arrow, source, destination,
                   (Vector2){box.width / 2.0f, box.height / 2.0f},
                   rotation, tint);
}

static void DrawCheatButton(Assets *asset, Rectangle button, const char *label,
                            bool hovered, bool enabled)
{
    Texture2D plate = (hovered && enabled)
        ? asset->texPauseButtonHover
        : asset->texPauseButtonNormal;
    DrawPauseButtonBase(plate, button, enabled ? WHITE : Fade(WHITE, 0.45f));
    if (hovered && enabled)
    {
        BeginBlendMode(BLEND_ADDITIVE);
        DrawPauseButtonBase(plate, button, COL_HOVER_LIFT);
        EndBlendMode();
    }
    DrawCheatTextCentered(asset->fontCondensed, label,
                          button.x + button.width / 2.0f,
                          button.y + button.height * 0.48f,
                          button.height * 0.40f,
                          enabled ? CHEAT_INK : Fade(CHEAT_INK, 0.45f));
}

static void DrawNavigationTab(Assets *asset, Rectangle tab, const char *label,
                              int direction, bool hovered, bool enabled)
{
    Color tint = enabled ? WHITE : Fade(WHITE, 0.40f);
    DrawWholeTinted(asset->texCheatTab, tab, tint);
    if (hovered && enabled)
    {
        BeginBlendMode(BLEND_ADDITIVE);
        DrawWholeTinted(asset->texCheatTab, tab, COL_HOVER_LIFT);
        EndBlendMode();
    }

    float arrowSize = tab.height * 0.34f;
    float inset = tab.width * 0.14f;
    Rectangle arrowBox = {
        direction == CHEAT_LEFT
            ? tab.x + inset
            : tab.x + tab.width - inset - arrowSize,
        tab.y + tab.height * 0.50f - arrowSize / 2.0f,
        arrowSize, arrowSize
    };
    DrawDirectionArrow(asset->texCheatArrow, arrowBox, direction,
                       enabled ? CHEAT_INK : Fade(CHEAT_INK, 0.40f));

    float labelCenterX = direction == CHEAT_LEFT
        ? tab.x + tab.width * 0.62f
        : tab.x + tab.width * 0.38f;
    DrawCheatTextCentered(asset->fontCondensed, label, labelCenterX,
                          tab.y + tab.height * 0.50f, tab.height * 0.26f,
                          enabled ? CHEAT_INK : Fade(CHEAT_INK, 0.40f));

}

static void DrawRouteRow(Assets *asset, const CheatRoute *route,
                         const RouteGrid *grid, int row, int firstStep,
                         int rowNumber)
{
    float rowY = grid->rows.y + grid->rowHeight * row;
    float centerY = rowY + grid->rowHeight / 2.0f;

    float ringRadius = grid->rowHeight * 0.235f;
    float ringX = grid->rows.x + grid->numberWidth * 0.42f;
    DrawRing((Vector2){ringX, centerY}, ringRadius * 0.86f, ringRadius,
             0.0f, 360.0f, 48, Fade(CHEAT_FAINT, 0.85f));
    DrawCheatTextCentered(asset->fontCondensed, TextFormat("%i", rowNumber),
                          ringX, centerY, ringRadius * 1.15f, CHEAT_FAINT);

    for (int i = 0; i < grid->unitsPerRow; i++)
    {
        int index = firstStep + i;
        if (index >= route->stepCount)
            break;

        float unitX = grid->rows.x + grid->numberWidth + grid->unitWidth * i;
        float arrowSize = grid->rowHeight * 0.54f;
        Rectangle arrowBox = {
            unitX, centerY - arrowSize / 2.0f, arrowSize, arrowSize
        };
        DrawDirectionArrow(asset->texCheatArrow, arrowBox,
                           route->step[index].direction, CHEAT_INK);

        if (route->step[index].repeat > 1)
        {
            float size = grid->rowHeight * 0.30f;
            const char *count = TextFormat("x%i", route->step[index].repeat);
            Vector2 measured = MeasureTextEx(asset->fontCondensed, count, size,
                                             size * CHEAT_TRACKING);
            DrawTextEx(asset->fontCondensed, count,
                       (Vector2){arrowBox.x + arrowSize * 1.06f,
                                 centerY - measured.y / 2.0f},
                       size, size * CHEAT_TRACKING, CHEAT_INK);
        }
    }

    if (row < ROUTE_ROWS - 1)
        DrawLineEx((Vector2){grid->rows.x + grid->numberWidth * 0.2f,
                             rowY + grid->rowHeight},
                   (Vector2){grid->rows.x + grid->rows.width,
                             rowY + grid->rowHeight},
                   1.5f, Fade(CHEAT_FAINT, 0.35f));
}

static void DrawRoute(Assets *asset, const CheatsheetLayout *layout,
                      const CheatsheetState *state, CheatsheetControl hovered)
{
    const CheatRoute *route = CheatsheetRoute(state->level);
    RouteGrid grid = GridOf(layout);
    int pages = CheatsheetPageCount(layout, state->level);
    int firstUnit = state->page * grid.unitsPerPage;

    for (int row = 0; row < ROUTE_ROWS; row++)
    {
        int firstStep = firstUnit + grid.unitsPerRow * row;
        if (firstStep >= route->stepCount)
            break;
        DrawRouteRow(asset, route, &grid, row, firstStep,
                     state->page * ROUTE_ROWS + row + 1);
    }

    if (pages <= 1)
        return;

    DrawCheatTextCentered(asset->fontCondensed,
                          TextFormat("PAGE %i / %i", state->page + 1, pages),
                          layout->paper.x + layout->paper.width / 2.0f,
                          layout->pagePrev.y +
                              layout->pagePrev.height / 2.0f,
                          layout->paper.height * 0.052f, CHEAT_FAINT);

    bool hasPrevious = state->page > 0;
    bool hasNext = state->page < pages - 1;
    DrawDirectionArrow(asset->texCheatArrow, layout->pagePrev, CHEAT_LEFT,
                       hasPrevious
                           ? (hovered == CHEAT_CONTROL_PAGE_PREV
                                  ? COL_NOIR_RED : CHEAT_INK)
                           : Fade(CHEAT_FAINT, 0.30f));
    DrawDirectionArrow(asset->texCheatArrow, layout->pageNext, CHEAT_RIGHT,
                       hasNext
                           ? (hovered == CHEAT_CONTROL_PAGE_NEXT
                                  ? COL_NOIR_RED : CHEAT_INK)
                           : Fade(CHEAT_FAINT, 0.30f));
}

static void DrawLockedPaper(Assets *asset, const CheatsheetLayout *layout)
{
    Rectangle box = {
        layout->paper.x + layout->paper.width * 0.16f,
        layout->paper.y + layout->paper.height * 0.10f,
        layout->paper.width * 0.68f,
        layout->paper.height * 0.62f
    };
    Texture2D lock = asset->texCheatLocked;
    float scale = box.width / lock.width;
    if (box.height / lock.height < scale)
        scale = box.height / lock.height;
    Rectangle destination = {
        box.x + (box.width - lock.width * scale) / 2.0f,
        box.y + (box.height - lock.height * scale) / 2.0f,
        lock.width * scale, lock.height * scale
    };
    DrawTexturePro(lock, (Rectangle){0, 0, (float)lock.width,
                                     (float)lock.height},
                   destination, (Vector2){0, 0}, 0, WHITE);

    float centerX = layout->paper.x + layout->paper.width / 2.0f;
    float titleY = layout->paper.y + layout->paper.height * 0.795f;
    DrawCheatTextCentered(asset->fontCondensed, "CASE LOCKED", centerX, titleY,
                          layout->paper.height * 0.085f, CHEAT_INK);
    DrawCheatTextCentered(asset->fontCondensed, "SOLVE THE PREVIOUS CASE",
                          centerX, titleY + layout->paper.height * 0.125f,
                          layout->paper.height * 0.050f, CHEAT_FAINT);
}

static void DrawRedactedPaper(Assets *asset, const CheatsheetLayout *layout)
{
    float centerX = layout->paper.x + layout->paper.width / 2.0f;
    float centerY = layout->paper.y + layout->paper.height * 0.44f;
    float size = layout->paper.height * 0.115f;

    Vector2 measured = MeasureTextEx(asset->fontCondensed, "ROUTE REDACTED",
                                     size, size * CHEAT_TRACKING);

    DrawRectangleRec((Rectangle){centerX - measured.x / 2.0f -
                                     layout->paper.width * 0.030f,
                                 centerY - measured.y * 0.62f,
                                 measured.x + layout->paper.width * 0.060f,
                                 measured.y * 1.24f},
                     Fade(CHEAT_INK, 0.90f));
    DrawCheatTextCentered(asset->fontCondensed, "ROUTE REDACTED", centerX,
                          centerY, size, COL_NOIR_IVORY);
    DrawCheatTextCentered(asset->fontCondensed,
                          "REVEAL ROUTE TO OPEN THE FILE", centerX,
                          centerY + layout->paper.height * 0.135f,
                          layout->paper.height * 0.050f, CHEAT_FAINT);
}

void CheatsheetDraw(Assets *asset, const CheatsheetLayout *layout,
                    const CheatsheetState *state, CheatsheetControl hovered)
{
    const CheatRoute *route = CheatsheetRoute(state->level);
    bool unlocked = LevelUnlocked(state->level);

    DrawWholeTinted(asset->texCheatBackground, layout->screen, WHITE);
    DrawWholeTinted(asset->texCheatNote, layout->note, WHITE);

    DrawPauseButtonBase(asset->texPauseButtonNormal, layout->title, WHITE);
    DrawCheatTextCentered(asset->fontCondensed, "CASE NOTES",
                          layout->title.x + layout->title.width / 2.0f,
                          layout->title.y + layout->title.height * 0.50f,
                          layout->title.height * 0.58f, CHEAT_INK);

    DrawPauseButtonBase(asset->texPauseButtonNormal, layout->levelPlate, WHITE);
    DrawCheatTextCentered(asset->fontCondensed,
                          TextFormat("LEVEL %02i", state->level + 1),
                          layout->levelPlate.x + layout->levelPlate.width / 2.0f,
                          layout->levelPlate.y + layout->levelPlate.height * 0.34f,
                          layout->levelPlate.height * 0.40f, CHEAT_INK);
    DrawCheatTextCentered(asset->fontCondensed,
                          TextFormat("OPTIMAL: %i MOVES  -  %i PUSHES",
                                     route->optimalMoves, route->optimalPushes),
                          layout->levelPlate.x + layout->levelPlate.width / 2.0f,
                          layout->levelPlate.y + layout->levelPlate.height * 0.71f,
                          layout->levelPlate.height * 0.235f, CHEAT_INK);

    DrawWholeTinted(asset->texCheatPanel, layout->panel, WHITE);

    if (!unlocked)
        DrawLockedPaper(asset, layout);
    else if (!state->revealed)
        DrawRedactedPaper(asset, layout);
    else
        DrawRoute(asset, layout, state, hovered);

    DrawNavigationTab(asset, layout->prevTab, "PREV", CHEAT_LEFT,
                      hovered == CHEAT_CONTROL_PREV, state->level > 0);
    DrawNavigationTab(asset, layout->nextTab, "NEXT", CHEAT_RIGHT,
                      hovered == CHEAT_CONTROL_NEXT,
                      state->level < LEVEL_COUNT - 1);

    DrawCheatButton(asset, layout->backButton, "BACK",
                    hovered == CHEAT_CONTROL_BACK, true);
    DrawCheatButton(asset, layout->revealButton,
                    state->revealed ? "HIDE ROUTE" : "REVEAL ROUTE",
                    hovered == CHEAT_CONTROL_REVEAL, unlocked);
}
