#include "controls.h"
#include "colors.h"

#include <stddef.h>

#define PAPER_X 0.088f
#define PAPER_Y 0.080f
#define PAPER_W 0.846f
#define PAPER_H 0.795f

#define UNITS_TOP 0.55f
#define UNITS_HEAD 1.45f
#define UNITS_BETWEEN 0.80f
#define UNITS_FOOT 0.90f
#define SHEET_UNITS 12.15f

static const Color CONTROLS_INK = {32, 28, 24, 255};
static const Color CONTROLS_FAINT = {96, 86, 76, 255};

#define CONTROLS_TRACKING 0.045f

#define CONTROLS_CAP_RATIO 0.55f

#define CAP_ARROWS "<ARROWS>"
#define CAP_LEFT "<LEFT>"
#define CAP_RIGHT "<RIGHT>"

#define CAP_OR "OR"

typedef struct ControlRow {

    const char *keys;
    const char *meaning;
} ControlRow;

typedef struct ControlBlock {
    const char *heading;
    int firstRow;
    int rowCount;
} ControlBlock;

static const ControlRow ROW[] = {
    {"<ARROWS>|OR|W|A|S|D", "WALK/PUSH"},
    {"U",                   "TAKE BACK A STEP"},
    {"R",                   "RESTART THE CASE"},
    {"P",                   "PAUSE"},
    {"M",                   "MUTE THE MUSIC"},

    {"ENTER",               "NEXT CASE"},
    {"R",                   "REPLAY THE CASE"}
};

static const ControlBlock BLOCK[] = {
    {"PLAYING",     0, 5},
    {"CASE CLOSED", 5, 2}
};

ControlsLayout ControlsGetLayout(Assets *asset, float screenWidth,
                                 float screenHeight)
{
    (void)asset;
    ControlsLayout layout = {0};
    const float W = screenWidth;
    const float H = screenHeight;

    layout.screen = (Rectangle){0, 0, W, H};

    float titleWidth = W * 0.320f;
    layout.title = (Rectangle){
        (W - titleWidth) / 2.0f, H * 0.030f, titleWidth, H * 0.098f
    };

    layout.panel = (Rectangle){W * 0.196f, H * 0.155f, W * 0.608f, H * 0.645f};
    layout.paper = (Rectangle){
        layout.panel.x + layout.panel.width * PAPER_X,
        layout.panel.y + layout.panel.height * PAPER_Y,
        layout.panel.width * PAPER_W,
        layout.panel.height * PAPER_H
    };

    float buttonWidth = W * 0.228f;
    float buttonHeight = H * 0.077f;
    layout.backButton = (Rectangle){
        (W - buttonWidth) / 2.0f, H * 0.860f, buttonWidth, buttonHeight
    };
    return layout;
}

ControlsControl ControlsHitTest(const ControlsLayout *layout, Vector2 mouse)
{
    if (CheckCollisionPointRec(mouse, layout->backButton))
        return CONTROLS_CONTROL_BACK;
    return CONTROLS_CONTROL_NONE;
}

static void DrawWholeTinted(Texture2D texture, Rectangle destination,
                            Color tint)
{
    Rectangle source = {0, 0, (float)texture.width, (float)texture.height};
    DrawTexturePro(texture, source, destination, (Vector2){0, 0}, 0, tint);
}

static void DrawRowText(Font font, const char *text, float x, float centerY,
                        float size, Color color)
{
    float spacing = size * CONTROLS_TRACKING;
    Vector2 measured = MeasureTextEx(font, text, size, spacing);
    DrawTextEx(font, text, (Vector2){x, centerY - measured.y / 2.0f},
               size, spacing, color);
}

static void DrawRowTextCentered(Font font, const char *text, float centerX,
                                float centerY, float size, Color color)
{
    float spacing = size * CONTROLS_TRACKING;
    Vector2 measured = MeasureTextEx(font, text, size, spacing);
    DrawTextEx(font, text,
               (Vector2){centerX - measured.x / 2.0f,
                         centerY - measured.y / 2.0f},
               size, spacing, color);
}

static void DrawArrowGlyph(Texture2D arrow, Rectangle box, float rotation,
                           Color tint)
{
    Rectangle source = {0, 0, (float)arrow.width, (float)arrow.height};
    Rectangle destination = {
        box.x + box.width / 2.0f, box.y + box.height / 2.0f,
        box.width, box.height
    };
    DrawTexturePro(arrow, source, destination,
                   (Vector2){box.width / 2.0f, box.height / 2.0f},
                   rotation, tint);
}

static float CapWidth(Font font, const char *label, float height)
{
    if (TextIsEqual(label, CAP_ARROWS))
        return height * 3.30f;

    float square = height * 1.12f;
    if (TextIsEqual(label, CAP_LEFT) || TextIsEqual(label, CAP_RIGHT))
        return square;

    float size = height * 0.46f / CONTROLS_CAP_RATIO;
    float text = MeasureTextEx(font, label, size, size * CONTROLS_TRACKING).x;
    float needed = text + height * 0.70f;
    return needed > square ? needed : square;
}

static void DrawKeyCap(Assets *asset, Rectangle cap, const char *label)
{
    float round = 0.26f;
    DrawRectangleRounded(cap, round, 6, Fade(CONTROLS_INK, 0.07f));
    DrawRectangleRoundedLinesEx(cap, round, 6, cap.height * 0.075f,
                                CONTROLS_INK);

    if (TextIsEqual(label, CAP_ARROWS))
    {

        float head = cap.height * 0.56f;
        float gap = head * 0.30f;
        float total = head * 4.0f + gap * 3.0f;
        float x = cap.x + (cap.width - total) / 2.0f;
        float y = cap.y + cap.height / 2.0f - head / 2.0f;
        static const float TURN[4] = {270.0f, 0.0f, 180.0f, 90.0f};
        for (int i = 0; i < 4; i++)
            DrawArrowGlyph(asset->texCheatArrow,
                           (Rectangle){x + i * (head + gap), y, head, head},
                           TURN[i], CONTROLS_INK);
        return;
    }
    if (TextIsEqual(label, CAP_LEFT) || TextIsEqual(label, CAP_RIGHT))
    {
        float head = cap.height * 0.52f;
        Rectangle box = {cap.x + (cap.width - head) / 2.0f,
                         cap.y + (cap.height - head) / 2.0f, head, head};
        DrawArrowGlyph(asset->texCheatArrow, box,
                       TextIsEqual(label, CAP_LEFT) ? 270.0f : 90.0f,
                       CONTROLS_INK);
        return;
    }
    DrawRowTextCentered(asset->fontCondensed, label,
                        cap.x + cap.width / 2.0f, cap.y + cap.height * 0.50f,
                        cap.height * 0.46f / CONTROLS_CAP_RATIO,
                        CONTROLS_INK);
}

static float RunKeyColumn(Assets *asset, const ControlRow *row, float x,
                          float centerY, float capHeight, bool draw)
{
    float start = x;
    float gap = capHeight * 0.28f;
    float capY = centerY - capHeight / 2.0f;
    float orSize = capHeight * 0.40f / CONTROLS_CAP_RATIO;

    int count = 0;
    const char *const *label =
        (const char *const *)TextSplit(row->keys, '|', &count);
    for (int i = 0; i < count; i++)
    {
        if (TextIsEqual(label[i], CAP_OR))
        {
            float width = MeasureTextEx(asset->fontCondensed, CAP_OR, orSize,
                                        orSize * CONTROLS_TRACKING).x;
            if (draw)
                DrawRowText(asset->fontCondensed, CAP_OR, x + gap, centerY,
                            orSize, CONTROLS_FAINT);
            x += width + gap * 2.0f;
            continue;
        }

        float width = CapWidth(asset->fontCondensed, label[i], capHeight);
        if (draw)
            DrawKeyCap(asset, (Rectangle){x, capY, width, capHeight},
                       label[i]);
        x += width + gap;
    }
    return x - start - gap;
}

static void DrawBlock(Assets *asset, const ControlBlock *block,
                      Rectangle area, float unit)
{
    float capHeight = unit * 0.68f;
    float headingSize = unit * 0.62f / CONTROLS_CAP_RATIO;
    float meaningSize = unit * 0.44f / CONTROLS_CAP_RATIO;

    DrawRowText(asset->fontCondensed, block->heading, area.x,
                area.y + unit * 0.38f, headingSize, CONTROLS_INK);

    float ruleY = area.y + unit * 0.95f;
    DrawLineEx((Vector2){area.x, ruleY},
               (Vector2){area.x + area.width, ruleY},
               1.8f, Fade(CONTROLS_FAINT, 0.45f));

    float keyWidth = 0.0f;
    for (int i = 0; i < block->rowCount; i++)
    {
        float width = RunKeyColumn(asset, &ROW[block->firstRow + i], 0.0f,
                                   0.0f, capHeight, false);
        if (width > keyWidth)
            keyWidth = width;
    }
    float meaningX = area.x + keyWidth + unit * 0.90f;

    float y = area.y + UNITS_HEAD * unit;
    for (int i = 0; i < block->rowCount; i++)
    {
        const ControlRow *row = &ROW[block->firstRow + i];
        float centerY = y + unit * 0.50f;
        RunKeyColumn(asset, row, area.x, centerY, capHeight, true);
        DrawRowText(asset->fontCondensed, row->meaning, meaningX, centerY,
                    meaningSize, CONTROLS_INK);
        y += unit;
    }
}

void ControlsDraw(Assets *asset, const ControlsLayout *layout,
                  ControlsControl hovered)
{
    DrawWholeTinted(asset->texCheatBackground, layout->screen, WHITE);

    DrawPauseButtonBase(asset->texPauseButtonNormal, layout->title, WHITE);
    DrawRowTextCentered(asset->fontCondensed, "CONTROLS",
                        layout->title.x + layout->title.width / 2.0f,
                        layout->title.y + layout->title.height * 0.50f,
                        layout->title.height * 0.58f, CONTROLS_INK);

    DrawWholeTinted(asset->texCheatPanel, layout->panel, WHITE);

    Rectangle paper = layout->paper;
    float unit = paper.height / SHEET_UNITS;
    float inset = paper.width * 0.045f;
    float left = paper.x + inset;
    float width = paper.width - inset * 2.0f;

    float y = paper.y + UNITS_TOP * unit;
    DrawBlock(asset, &BLOCK[0], (Rectangle){left, y, width, 0}, unit);
    y += (UNITS_HEAD + (float)BLOCK[0].rowCount + UNITS_BETWEEN) * unit;

    DrawBlock(asset, &BLOCK[1], (Rectangle){left, y, width, 0}, unit);
    y += (UNITS_HEAD + (float)BLOCK[1].rowCount) * unit;

    DrawRowTextCentered(asset->fontCondensed,
                        "HELD KEYS REPEAT AT THE SPEED SET IN SETTINGS",
                        paper.x + paper.width / 2.0f,
                        y + UNITS_FOOT * unit * 0.55f,
                        unit * 0.38f / CONTROLS_CAP_RATIO, CONTROLS_FAINT);

    Texture2D plate = hovered == CONTROLS_CONTROL_BACK
        ? asset->texPauseButtonHover
        : asset->texPauseButtonNormal;
    DrawPauseButtonBase(plate, layout->backButton, WHITE);
    if (hovered == CONTROLS_CONTROL_BACK)
    {

        BeginBlendMode(BLEND_ADDITIVE);
        DrawPauseButtonBase(plate, layout->backButton,
                            COL_HOVER_LIFT);
        EndBlendMode();
    }
    DrawRowTextCentered(asset->fontCondensed, "BACK",
                        layout->backButton.x + layout->backButton.width / 2.0f,
                        layout->backButton.y + layout->backButton.height * 0.48f,
                        layout->backButton.height * 0.40f, CONTROLS_INK);
}
