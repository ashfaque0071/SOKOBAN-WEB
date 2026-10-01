// The credits page: the people behind the game and the sounds and artwork
// that give this case its Spider-Noir identity.

#include "credits.h"
#include "colors.h"

#include <stddef.h>

//---------------------------------------------------------------- LAYOUT -----
#define PAPER_X 0.088f
#define PAPER_Y 0.145f
#define PAPER_W 0.846f
#define PAPER_H 0.720f

// The page is measured in rows so the layout scales with the window while
// keeping the requested hierarchy and spacing intact.
#define UNITS_TOP 0.35f
#define UNITS_HEAD 1.10f
#define UNITS_BETWEEN 0.55f
#define UNITS_FOOT 0.25f
#define DESIGN_ROW 1.00f
#define SUPERVISION_HEAD 1.00f
#define SUPERVISION_NAME 1.00f

static const Color CREDITS_INK = {32, 28, 24, 255};
static const Color CREDITS_FAINT = {96, 86, 76, 255};

#define CREDITS_TRACKING 0.045f
#define CREDITS_CAP_RATIO 0.55f

typedef struct CreditRow {
    const char *name;
    const char *note;
} CreditRow;

typedef struct CreditBlock {
    const char *heading;
    int firstRow;
    int rowCount;
    bool designBlock;
} CreditBlock;

static const CreditRow ROW[] = {
    {"ASHFAQUE AHMED NUR(2505103)", NULL},
    {"IMTIAZ AHMED(2505113)", NULL},

    {"BG MUSIC",         "SPIDER-MAN THEME SONG(INSTRUMENTAL)"},
    {"",                   ""},
    {"SFX LEVEL CLEAR",  "VENOM THEME SONG"},
    {"ASSETS",           "SPIDER-NOIR THEMED"}
};

static const CreditBlock BLOCK[] = {
    {"GAME DESIGN AND CODE", 0, 2, true},
    {"SOUND AND ASSETS",     2, 3, false}
};

#define BLOCK_COUNT ((int)(sizeof(BLOCK) / sizeof(BLOCK[0])))

static float SheetUnits(void)
{
    float units = UNITS_TOP + UNITS_FOOT;
    for (int i = 0; i < BLOCK_COUNT; i++)
    {
        units += UNITS_HEAD + (float)BLOCK[i].rowCount *
                 (BLOCK[i].designBlock ? DESIGN_ROW : 1.0f);
        units += UNITS_BETWEEN;
    }
    units += SUPERVISION_HEAD + SUPERVISION_NAME;
    return units;
}

CreditsLayout CreditsGetLayout(Assets *asset, float screenWidth,
                               float screenHeight)
{
    (void)asset;
    CreditsLayout layout = {0};
    const float W = screenWidth;
    const float H = screenHeight;

    layout.screen = (Rectangle){0, 0, W, H};

    float titleWidth = W * 0.320f;
    layout.title = (Rectangle){
        (W - titleWidth) / 2.0f, H * 0.018f, titleWidth, H * 0.085f
    };

    layout.panel = (Rectangle){W * 0.170f, H * 0.118f, W * 0.660f, H * 0.700f};
    layout.paper = (Rectangle){
        layout.panel.x + layout.panel.width * PAPER_X,
        layout.panel.y + layout.panel.height * PAPER_Y,
        layout.panel.width * PAPER_W,
        layout.panel.height * PAPER_H
    };

    float buttonWidth = W * 0.228f;
    float buttonHeight = H * 0.072f;
    layout.backButton = (Rectangle){
        (W - buttonWidth) / 2.0f, H * 0.855f, buttonWidth, buttonHeight
    };
    return layout;
}

CreditsControl CreditsHitTest(const CreditsLayout *layout, Vector2 mouse)
{
    if (CheckCollisionPointRec(mouse, layout->backButton))
        return CREDITS_CONTROL_BACK;
    return CREDITS_CONTROL_NONE;
}

//--------------------------------------------------------------- DRAWING -----

static void DrawWholeTinted(Texture2D texture, Rectangle destination,
                            Color tint)
{
    Rectangle source = {0, 0, (float)texture.width, (float)texture.height};
    DrawTexturePro(texture, source, destination, (Vector2){0, 0}, 0, tint);
}

static void DrawRowText(Font font, const char *text, float x, float centerY,
                        float size, Color color)
{
    float spacing = size * CREDITS_TRACKING;
    Vector2 measured = MeasureTextEx(font, text, size, spacing);
    DrawTextEx(font, text, (Vector2){x, centerY - measured.y / 2.0f},
               size, spacing, color);
}

static void DrawRowTextRight(Font font, const char *text, float rightX,
                             float centerY, float size, Color color)
{
    float spacing = size * CREDITS_TRACKING;
    Vector2 measured = MeasureTextEx(font, text, size, spacing);
    DrawTextEx(font, text,
               (Vector2){rightX - measured.x, centerY - measured.y / 2.0f},
               size, spacing, color);
}

static void DrawRowTextCentered(Font font, const char *text, float centerX,
                                float centerY, float size, Color color)
{
    float spacing = size * CREDITS_TRACKING;
    Vector2 measured = MeasureTextEx(font, text, size, spacing);
    DrawTextEx(font, text,
               (Vector2){centerX - measured.x / 2.0f,
                         centerY - measured.y / 2.0f},
               size, spacing, color);
}

// Keep the long Spider-Man theme label inside the paper at smaller window
// sizes without changing the visual size of the shorter labels.
static float FitTextSize(Font font, const char *text, float wanted,
                         float maxWidth)
{
    float size = wanted;
    while (size > 8.0f &&
           MeasureTextEx(font, text, size, size * CREDITS_TRACKING).x >
               maxWidth)
        size *= 0.94f;
    return size;
}

static void DrawBlock(Assets *asset, const CreditBlock *block,
                      Rectangle area, float unit)
{
    float headingSize = unit * 0.56f / CREDITS_CAP_RATIO;
    float rowHeight = block->designBlock ? DESIGN_ROW : 1.0f;
    float right = area.x + area.width;

    DrawRowText(asset->fontCondensed, block->heading, area.x,
                area.y + unit * 0.36f, headingSize, CREDITS_INK);

    float ruleY = area.y + unit * 0.84f;
    DrawLineEx((Vector2){area.x, ruleY}, (Vector2){right, ruleY},
               1.8f, Fade(CREDITS_FAINT, 0.45f));

    Font nameFont = block->designBlock
        ? asset->fontNoir : asset->fontCondensed;
    float nameSize = block->designBlock
        ? unit * 0.64f
        : unit * 0.32f / CREDITS_CAP_RATIO;
    float noteSize = unit * 0.30f / CREDITS_CAP_RATIO;

    float y = area.y + UNITS_HEAD * unit;
    for (int i = 0; i < block->rowCount; i++)
    {
        const CreditRow *row = &ROW[block->firstRow + i];
        float centerY = y + rowHeight * unit * 0.50f;

        if (block->designBlock)
        {
            float fitted = FitTextSize(nameFont, row->name, nameSize,
                                       area.width * 0.85f);
            DrawRowText(nameFont, row->name, area.x, centerY, fitted,
                        CREDITS_INK);
        }
        else
        {
            DrawRowText(asset->fontCondensed, row->name, area.x, centerY,
                        nameSize, CREDITS_INK);
            float fitted = FitTextSize(asset->fontCondensed, row->note,
                                       noteSize, area.width * 0.70f);
            DrawRowTextRight(asset->fontCondensed, row->note, right,
                             centerY, fitted, CREDITS_FAINT);
        }
        y += rowHeight * unit;
    }
}

void CreditsDraw(Assets *asset, const CreditsLayout *layout,
                 CreditsControl hovered)
{
    DrawWholeTinted(asset->texCheatBackground, layout->screen, WHITE);

    DrawPauseButtonBase(asset->texPauseButtonNormal, layout->title, WHITE);
    DrawRowTextCentered(asset->fontCondensed, "CREDITS",
                        layout->title.x + layout->title.width / 2.0f,
                        layout->title.y + layout->title.height * 0.50f,
                        layout->title.height * 0.58f, CREDITS_INK);

    DrawWholeTinted(asset->texCheatPanel, layout->panel, WHITE);

    Rectangle paper = layout->paper;
    float unit = paper.height / SheetUnits();
    float left = paper.x + paper.width * 0.065f;
    float width = paper.width * (1.0f - 0.065f - 0.115f);
    float centerX = paper.x + paper.width / 2.0f;

    float y = paper.y + UNITS_TOP * unit;
    for (int i = 0; i < BLOCK_COUNT; i++)
    {
        DrawBlock(asset, &BLOCK[i], (Rectangle){left, y, width, 0}, unit);
        y += (UNITS_HEAD + (float)BLOCK[i].rowCount *
              (BLOCK[i].designBlock ? DESIGN_ROW : 1.0f)) * unit;
        y += UNITS_BETWEEN * unit;
    }

    DrawRowTextCentered(asset->fontCondensed, "UNDER THE SUPERVISION OF",
                        centerX, y + unit * SUPERVISION_HEAD * 0.40f,
                        unit * 0.36f / CREDITS_CAP_RATIO, CREDITS_INK);
    DrawRowTextCentered(asset->fontNoir, "JUNAED YOUNOUS KHAN", centerX,
                        y + unit * (SUPERVISION_HEAD +
                                    SUPERVISION_NAME * 0.50f),
                        unit * 0.62f, CREDITS_INK);

    Texture2D plate = hovered == CREDITS_CONTROL_BACK
        ? asset->texPauseButtonHover
        : asset->texPauseButtonNormal;
    DrawPauseButtonBase(plate, layout->backButton, WHITE);
    if (hovered == CREDITS_CONTROL_BACK)
    {
        BeginBlendMode(BLEND_ADDITIVE);
        DrawPauseButtonBase(plate, layout->backButton, COL_HOVER_LIFT);
        EndBlendMode();
    }
    DrawRowTextCentered(asset->fontCondensed, "BACK",
                        layout->backButton.x + layout->backButton.width / 2.0f,
                        layout->backButton.y +
                            layout->backButton.height * 0.48f,
                        layout->backButton.height * 0.40f, CREDITS_INK);
}
