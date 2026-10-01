#include "levelselect.h"

#include "colors.h"
#include "score.h"
#include "screen.h"
#include "ui.h"

int LevelSelectSlotCount(void)
{
    return LEVEL_COUNT < LEVEL_SLOTS ? LEVEL_COUNT : LEVEL_SLOTS;
}

LevelSelectLayout LevelSelectGetLayout(Assets *asset)
{
    LevelSelectLayout layout = {0};

    for (int i = 0; i < LEVEL_SLOTS; i++)
    {
        int column = i % LEVEL_COLUMNS;
        int row = i / LEVEL_COLUMNS;
        layout.card[i] = (Rectangle){
            SCREEN_W * (0.2129f + 0.1459f * column),
            SCREEN_H * (0.2115f + 0.1650f * row),
            SCREEN_W * 0.1334f,
            SCREEN_H * 0.1456f
        };
    }

    float bannerWidth = SCREEN_W * 0.271f;
    layout.banner = (Rectangle){
        SCREEN_W * 0.498f - bannerWidth / 2.0f, SCREEN_H * 0.012f,
        bannerWidth,
        bannerWidth * asset->texLevelsBanner.height / asset->texLevelsBanner.width
    };

    float tagWidth = SCREEN_W * 0.158f;
    layout.starTag = (Rectangle){
        SCREEN_W * 0.652f, SCREEN_H * 0.026f, tagWidth,
        tagWidth * asset->texLevelsStarTag.height / asset->texLevelsStarTag.width
    };

    float backWidth = SCREEN_W * 0.0873f;
    layout.backButton = (Rectangle){
        SCREEN_W * 0.0293f, SCREEN_H * 0.018f, backWidth,
        backWidth * asset->texLevelsButtonBack.height /
            asset->texLevelsButtonBack.width
    };

    float controlsWidth = SCREEN_W * 0.075f;
    layout.controlsButton = (Rectangle){
        SCREEN_W * 0.898f, SCREEN_H * 0.018f, controlsWidth,
        controlsWidth * asset->texLevelsButtonControls.height /
            asset->texLevelsButtonControls.width
    };

    float shelfHeight = SCREEN_H * 0.094f;
    float shelfWidth = shelfHeight * 3.55f;
    float shelfGap = SCREEN_W * 0.020f;
    float shelfX = (SCREEN_W - (shelfWidth * 2.0f + shelfGap)) / 2.0f;
    float shelfY = SCREEN_H * 0.864f;

    layout.highScoresButton = (Rectangle){
        shelfX, shelfY, shelfWidth, shelfHeight
    };
    layout.cheatsheetsButton = (Rectangle){
        shelfX + shelfWidth + shelfGap, shelfY, shelfWidth, shelfHeight
    };
    return layout;
}

LevelSelectHover LevelSelectHitTest(const LevelSelectLayout *layout,
                                    Vector2 mouse)
{
    LevelSelectHover hover = {-1, -1, -1, false, false};
    int shown = LevelSelectSlotCount();

    for (int i = 0; i < shown; i++)
    {
        if (!CheckCollisionPointRec(mouse, layout->card[i]))
            continue;
        if (LevelUnlocked(i))
            hover.card = i;
        else
            hover.lockedCard = i;
    }

    if (CheckCollisionPointRec(mouse, layout->highScoresButton))
        hover.shelf = LEVEL_SHELF_HIGH_SCORES;
    else if (CheckCollisionPointRec(mouse, layout->cheatsheetsButton))
        hover.shelf = LEVEL_SHELF_CHEATSHEETS;

    hover.back = CheckCollisionPointRec(mouse, layout->backButton);
    hover.controls = CheckCollisionPointRec(mouse, layout->controlsButton);
    return hover;
}

bool LevelSelectHoveringControl(LevelSelectHover hover)
{
    return hover.card >= 0 || hover.shelf >= 0 || hover.back || hover.controls;
}

static void DrawBoardButton(Assets *asset, Rectangle button, Texture2D icon,
                            const char *label, bool hovered)
{
    Texture2D plate = hovered
        ? asset->texPauseButtonHover
        : asset->texPauseButtonNormal;
    DrawPaperPlate(plate, PauseInk(plate, 0.023f, 0.153f, 0.960f, 0.692f),
                   button);

    float iconHeight = button.height * 0.50f;
    float iconWidth = iconHeight * icon.width / icon.height;
    float iconX = button.x + button.width * 0.12f;
    DrawWhole(icon, (Rectangle){
        iconX, button.y + (button.height - iconHeight) / 2.0f,
        iconWidth, iconHeight
    });

    float size = button.height * 0.40f;
    float spacing = size * 0.045f;
    float labelX = iconX + iconWidth + button.width * 0.055f;

    float room = button.x + button.width * 0.88f - labelX;
    Vector2 widest = MeasureTextEx(asset->fontCondensed, "CHEATSHEETS",
                                   size, spacing);
    if (widest.x > room)
    {
        float fit = room / widest.x;
        size *= fit;
        spacing *= fit;
    }

    Vector2 measured = MeasureTextEx(asset->fontCondensed, label,
                                     size, spacing);
    DrawTextEx(asset->fontCondensed, label,
               (Vector2){labelX, button.y + (button.height - measured.y) / 2.0f},
               size, spacing, (Color){32, 28, 24, 255});
}

static void DrawLevelStars(Assets *asset, Rectangle card, int stars)
{
    float starSize = card.width * 0.183f;
    float gap = card.width * 0.018f;
    float totalWidth = starSize * 3.0f + gap * 2.0f;
    float x = card.x + (card.width - totalWidth) / 2.0f;
    float y = card.y + card.height * 0.575f;

    for (int i = 0; i < 3; i++)
    {
        Texture2D star = i < stars
            ? asset->texLevelsStarGold
            : asset->texLevelsStarGrey;
        Rectangle box = {x + i * (starSize + gap), y, starSize, starSize};
        DrawWhole(star, FitInside(star, box));
    }
}

#define CARD_LIFT_ALPHA 26

static void LiftLevelCard(Rectangle card)
{
    BeginBlendMode(BLEND_ADDITIVE);
    DrawRectangleRounded(card, 0.10f, 8, COL_HOVER_LIFT_AT(CARD_LIFT_ALPHA));
    EndBlendMode();
}

static void DrawLevelCard(const Board *board, Assets *asset, Rectangle card,
                          int index, bool hovered)
{
    Color ink = (Color){31, 27, 24, 255};

    if (!LevelUnlocked(index))
    {

        Rectangle cover = {
            card.x - 3, card.y - 3, card.width + 6, card.height + 6
        };
        DrawWhole(asset->texLevelsCardLocked, cover);

        DrawFontCenteredInRectangle(
            asset->fontNoir, TextFormat("%i", index + 1),
            (Rectangle){card.x + card.width * 0.05f,
                        card.y + card.height * 0.32f,
                        card.width * 0.26f, card.height * 0.34f},
            card.height * 0.36f, 1.0f, (Color){214, 206, 188, 255}, 0
        );

        float lockHeight = card.height * 0.62f;
        float lockWidth = lockHeight * asset->texLevelsPadlock.width /
                          asset->texLevelsPadlock.height;
        DrawWhole(asset->texLevelsPadlock, (Rectangle){
            card.x + card.width * 0.50f - lockWidth / 2.0f,
            card.y + card.height * 0.50f - lockHeight / 2.0f,
            lockWidth, lockHeight
        });
        return;
    }

    if (hovered)
        LiftLevelCard(card);

    bool cleared = LevelCleared(index);

    DrawFontCenteredInRectangle(
        asset->fontNoir, TextFormat("%i", index + 1),
        (Rectangle){card.x, card.y + card.height * 0.08f,
                    card.width, card.height * 0.42f},
        card.height * 0.46f, 1.0f, ink, 0
    );

    if (cleared)
    {
        DrawLevelStars(asset, card, LevelBestStars(index));
        return;
    }

    float stampWidth = card.width * 0.70f;
    DrawWhole(asset->texLevelsStampProgress, (Rectangle){
        card.x + (card.width - stampWidth) / 2.0f,
        card.y + card.height * 0.44f, stampWidth,
        stampWidth * asset->texLevelsStampProgress.height /
            asset->texLevelsStampProgress.width
    });

    if (index == board->currentLevel)
    {
        DrawFontCenteredInRectangle(
            asset->fontNoir, TextFormat("%i PUSHES", board->pushCount),
            (Rectangle){card.x, card.y + card.height * 0.78f,
                        card.width, card.height * 0.18f},
            card.height * 0.16f, 0.5f, ink, 0
        );
    }
}

void LevelSelectDraw(const Board *board, Assets *asset,
                     const LevelSelectLayout *layout,
                     LevelSelectHover hover)
{

    DrawWhole(asset->texLevelsBackground,
              (Rectangle){0, 0, SCREEN_W, SCREEN_H});
    DrawWhole(asset->texLevelsBanner, layout->banner);
    DrawLitTexture(asset->texLevelsButtonBack, layout->backButton,
                   hover.back);
    DrawLitTexture(asset->texLevelsButtonControls, layout->controlsButton,
                   hover.controls);

    DrawBoardButton(asset, layout->highScoresButton,
                    asset->texLevelsStarGold, "HIGH SCORES",
                    hover.shelf == LEVEL_SHELF_HIGH_SCORES);
    DrawBoardButton(asset, layout->cheatsheetsButton,
                    asset->texLevelsIconMap, "CHEATSHEETS",
                    hover.shelf == LEVEL_SHELF_CHEATSHEETS);

    int shown = LevelSelectSlotCount();
    int earned = 0;
    for (int i = 0; i < shown; i++)
        earned += LevelBestStars(i);

    DrawWhole(asset->texLevelsStarTag, layout->starTag);

    float starSize = layout->starTag.height * 0.46f;
    Rectangle starBox = {
        layout->starTag.x + layout->starTag.width * 0.08f,
        layout->starTag.y + layout->starTag.height * 0.27f,
        starSize, starSize
    };
    DrawWhole(asset->texLevelsStarGold,
              FitInside(asset->texLevelsStarGold, starBox));
    DrawFontCenteredInRectangle(
        asset->fontNoir, TextFormat("%i / %i", earned, shown * 3),
        (Rectangle){layout->starTag.x + layout->starTag.width * 0.34f,
                    layout->starTag.y + layout->starTag.height * 0.24f,
                    layout->starTag.width * 0.58f,
                    layout->starTag.height * 0.28f},
        layout->starTag.height * 0.26f, 0.5f, (Color){31, 27, 24, 255}, 0
    );
    DrawFontCenteredInRectangle(
        asset->fontNoir, "STARS",
        (Rectangle){layout->starTag.x + layout->starTag.width * 0.34f,
                    layout->starTag.y + layout->starTag.height * 0.54f,
                    layout->starTag.width * 0.58f,
                    layout->starTag.height * 0.24f},
        layout->starTag.height * 0.20f, 0.5f, (Color){31, 27, 24, 255}, 0
    );

    for (int i = 0; i < shown; i++)
        DrawLevelCard(board, asset, layout->card[i], i, i == hover.card);
}
