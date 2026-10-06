#include "hud.h"

#include "audio.h"
#include "colors.h"
#include "score.h"
#include "screen.h"
#include "ui.h"

HudLayout HudGetLayout(void)
{
    HudLayout layout = {0};

    int margin = SCREEN_W / 50;
    int headerHeight = SCREEN_H / 8;
    int boardAreaX = margin;
    int boardAreaY = headerHeight + margin / 2;
    int boardAreaW = SCREEN_W - margin * 2;
    int boardAreaH = SCREEN_H - boardAreaY - margin;

    int tileW = boardAreaW / COLS;
    int tileH = boardAreaH / ROWS;
    int tile = tileW;
    if (tileH < tileW)
        tile = tileH;
    if (tile > 68)
        tile = 68;

    layout.tile = tile;
    layout.originX = boardAreaX + (boardAreaW - COLS * tile) / 2;
    layout.originY = boardAreaY + (boardAreaH - ROWS * tile) / 2;

    int panelHeight = headerHeight - margin;
    int panelY = margin / 2;
    int iconSize = panelHeight;
    int buttonGap = margin / 3;

    int controlsWidth = iconSize * 4 + buttonGap * 3;
    int controlsX = SCREEN_W - margin - controlsWidth;
    int statsToControlsGap = margin * 4;
    int statsWidth = controlsX - margin - statsToControlsGap;
    int statWidth = statsWidth / 4;

    layout.panelHeight = panelHeight;
    layout.panelY = panelY;
    layout.infoFont = SCREEN_H / 32;
    layout.controlsX = controlsX;

    layout.statsPanel = (Rectangle){margin, panelY, statsWidth, panelHeight};
    layout.levelPanel = (Rectangle){
        layout.statsPanel.x, panelY, statWidth, panelHeight
    };
    layout.movePanel = (Rectangle){
        layout.statsPanel.x + statWidth, panelY, statWidth, panelHeight
    };
    layout.pushPanel = (Rectangle){
        layout.statsPanel.x + statWidth * 2, panelY, statWidth, panelHeight
    };
    layout.bestPanel = (Rectangle){
        layout.statsPanel.x + statWidth * 3, panelY,
        statsWidth - statWidth * 3, panelHeight
    };

    int iconY = panelY + (panelHeight - iconSize) / 2;
    layout.hintButton = (Rectangle){controlsX, iconY, iconSize, iconSize};
    layout.undoButton = (Rectangle){
        controlsX + iconSize + buttonGap, iconY, iconSize, iconSize
    };
    layout.restartButton = (Rectangle){
        controlsX + (iconSize + buttonGap) * 2, iconY, iconSize, iconSize
    };
    layout.pauseButton = (Rectangle){
        controlsX + (iconSize + buttonGap) * 3, iconY, iconSize, iconSize
    };

    int boardPadding = margin / 3;
    layout.boardFrame = (Rectangle){
        layout.originX - boardPadding, layout.originY - boardPadding,
        COLS * tile + boardPadding * 2, ROWS * tile + boardPadding * 2
    };
    return layout;
}

static void DrawGameBackground(Assets *asset)
{
    float scaleX = (float)SCREEN_W / asset->texGameBackground.width;
    float scaleY = (float)SCREEN_H / asset->texGameBackground.height;
    float scale = scaleX > scaleY ? scaleX : scaleY;
    float sourceWidth = SCREEN_W / scale;
    float sourceHeight = SCREEN_H / scale;

    Rectangle source = {
        (asset->texGameBackground.width - sourceWidth) / 2,
        (asset->texGameBackground.height - sourceHeight) / 2,
        sourceWidth,
        sourceHeight};
    Rectangle destination = {0, 0, SCREEN_W, SCREEN_H};

    DrawTexturePro(asset->texGameBackground, source, destination, (Vector2){0, 0}, 0, WHITE);
}

static void DrawHudPanel(Assets *asset, Rectangle destination)
{
    float sourceCapWidth = 180.0f;
    float destinationCapWidth =
        destination.height * sourceCapWidth / asset->texHudPanel.height;

    Rectangle leftSource = {0, 0, sourceCapWidth, asset->texHudPanel.height};
    Rectangle centerSource = {
        sourceCapWidth,
        0,
        asset->texHudPanel.width - sourceCapWidth * 2,
        asset->texHudPanel.height
    };
    Rectangle rightSource = {
        asset->texHudPanel.width - sourceCapWidth,
        0,
        sourceCapWidth,
        asset->texHudPanel.height
    };

    Rectangle leftDestination = {
        destination.x, destination.y,
        destinationCapWidth, destination.height
    };
    Rectangle centerDestination = {
        destination.x + destinationCapWidth,
        destination.y,
        destination.width - destinationCapWidth * 2,
        destination.height
    };
    Rectangle rightDestination = {
        destination.x + destination.width - destinationCapWidth,
        destination.y,
        destinationCapWidth, destination.height
    };

    DrawTexturePro(asset->texHudPanel, leftSource, leftDestination,
                   (Vector2){0, 0}, 0, WHITE);
    DrawTexturePro(asset->texHudPanel, centerSource, centerDestination,
                   (Vector2){0, 0}, 0, WHITE);
    DrawTexturePro(asset->texHudPanel, rightSource, rightDestination,
                   (Vector2){0, 0}, 0, WHITE);
}

static void DrawTextInPanel(const char *text, Rectangle panel, int size,
                            Color color)
{
    int textWidth = MeasureText(text, size);
    DrawText(text, panel.x + (panel.width - textWidth) / 2,
             panel.y + (panel.height - size) / 2, size, color);
}

static void DrawUiIcon(Assets *asset, int column, int row,
                       Rectangle destination, Color tint)
{
    float iconWidth = asset->texUiIcons.width / 3.0f;
    float iconHeight = asset->texUiIcons.height / 2.0f;
    Rectangle source = {
        column * iconWidth, row * iconHeight, iconWidth, iconHeight
    };

    /* Each icon sits inside its cell with its own margins. These were measured
       off the 1537x1023 sheet and are kept as fractions of a cell, not pixel
       counts, so the slices stay correct if the sheet ever ships at another
       resolution (the browser build loads a smaller copy). */
    if (column == 0 && row == 0)
        source = (Rectangle){
            iconWidth * 0.275211f, iconHeight * 0.175953f,
            iconWidth * 0.595316f, iconHeight * 0.690127f
        };
    else if (column == 1 && row == 0)
        source = (Rectangle){
            iconWidth + iconWidth * 0.193234f, iconHeight * 0.217009f,
            iconWidth * 0.614834f, iconHeight * 0.633431f
        };
    else if (column == 0 && row == 1)
        source = (Rectangle){
            iconWidth * 0.337671f, iconHeight + iconHeight * 0.144673f,
            iconWidth * 0.456734f, iconHeight * 0.576735f
        };
    else if (column == 2 && row == 0)
        source = (Rectangle){
            iconWidth * 2 + iconWidth * 0.206897f, iconHeight * 0.211144f,
            iconWidth * 0.501627f, iconHeight * 0.639296f
        };

    float scaleX = destination.width / iconWidth;
    float scaleY = destination.height / iconHeight;
    float scale = scaleX < scaleY ? scaleX : scaleY;
    float drawWidth = source.width * scale;
    float drawHeight = source.height * scale;
    Rectangle centeredDestination = {
        destination.x + (destination.width - drawWidth) / 2,
        destination.y + (destination.height - drawHeight) / 2,
        drawWidth,
        drawHeight
    };

    DrawTexturePro(asset->texUiIcons, source, centeredDestination,
                   (Vector2){0, 0}, 0, tint);
}

static void DrawUiButtonLift(Assets *asset, int iconColumn, int iconRow,
                             Rectangle destination, Color lift)
{
    Rectangle plate = {
        0, 0, asset->texUiButton.width, asset->texUiButton.height
    };
    float inset = destination.width * 0.16f;
    Rectangle iconDestination = {
        destination.x + inset,
        destination.y + inset,
        destination.width - inset * 2,
        destination.height - inset * 2
    };

    BeginBlendMode(BLEND_ADDITIVE);
    DrawTexturePro(asset->texUiButton, plate, destination,
                   (Vector2){0, 0}, 0, lift);
    DrawUiIcon(asset, iconColumn, iconRow, iconDestination, lift);
    EndBlendMode();
}

static void DrawUiButton(Assets *asset, int iconColumn, int iconRow,
                         Rectangle destination, bool enabled, bool hovered)
{
    Color tint = enabled ? WHITE : Fade(WHITE, 0.35f);
    Rectangle source = {
        0, 0, asset->texUiButton.width, asset->texUiButton.height
    };
    DrawTexturePro(asset->texUiButton, source, destination,
                   (Vector2){0, 0}, 0, tint);

    float inset = destination.width * 0.16f;
    Rectangle iconDestination = {
        destination.x + inset,
        destination.y + inset,
        destination.width - inset * 2,
        destination.height - inset * 2
    };
    DrawUiIcon(asset, iconColumn, iconRow, iconDestination, tint);

    if (enabled && hovered)
        DrawUiButtonLift(asset, iconColumn, iconRow, destination,
                         COL_HOVER_LIFT);
}

void HudDraw(const Board *board, Assets *asset, const HudLayout *layout,
             const Hint *hint, HudHover hover)
{
    Color ink = (Color){35, 31, 28, 255};
    int panelY = layout->panelY;
    int panelHeight = layout->panelHeight;
    int infoFont = layout->infoFont;

    DrawGameBackground(asset);

    DrawHudPanel(asset, layout->statsPanel);
    DrawTextInPanel(TextFormat("LEVEL %i / %i", board->currentLevel + 1,
                               LEVEL_COUNT),
                    layout->levelPanel, infoFont, ink);
    DrawTextInPanel(TextFormat("%i MOVES", board->moveCount),
                    layout->movePanel, infoFont, ink);
    DrawTextInPanel(TextFormat("%i PUSHES", board->pushCount),
                    layout->pushPanel, infoFont, ink);

    char bestText[24];
    if (LevelBestScore(board->currentLevel) > 0)
    {
        FormatScore(bestText, LevelBestScore(board->currentLevel));
        TextCopy(bestText, TextFormat("BEST %s", bestText));
    }
    else
    {
        TextCopy(bestText, "BEST --");
    }
    DrawTextInPanel(bestText, layout->bestPanel, infoFont, ink);

    Color separatorColor = (Color){35, 31, 28, 180};
    DrawLineEx((Vector2){layout->movePanel.x, panelY + 16},
               (Vector2){layout->movePanel.x, panelY + panelHeight - 16},
               2.0f, separatorColor);
    DrawLineEx((Vector2){layout->pushPanel.x, panelY + 16},
               (Vector2){layout->pushPanel.x, panelY + panelHeight - 16},
               2.0f, separatorColor);
    DrawLineEx((Vector2){layout->bestPanel.x, panelY + 16},
               (Vector2){layout->bestPanel.x, panelY + panelHeight - 16},
               2.0f, separatorColor);

    DrawRectangleRec(layout->boardFrame, (Color){13, 15, 22, 255});
    DrawRectangleLinesEx(layout->boardFrame, 4, (Color){55, 45, 37, 255});
    BoardDraw(board, asset, layout->originX, layout->originY, layout->tile);

    HintDraw(hint, layout->originX, layout->originY, layout->tile);

    DrawUiButton(asset, 2, 0, layout->hintButton, true,
                 hover.hint && !hint->active);
    if (hint->active)
        DrawUiButtonLift(asset, 2, 0, layout->hintButton, COL_HOVER_LIFT);
    DrawUiButton(asset, 0, 0, layout->undoButton, board->historyCount > 0,
                 hover.undo);
    DrawUiButton(asset, 1, 0, layout->restartButton, true, hover.restart);
    DrawUiButton(asset, 0, 1, layout->pauseButton, true, hover.pause);

    if (hint->status == HINT_DEAD)
        HintDrawDeadNotice(asset, (Rectangle){
            layout->boardFrame.x + layout->boardFrame.width * 0.20f,
            layout->boardFrame.y + layout->boardFrame.height * 0.055f,
            layout->boardFrame.width * 0.60f,
            layout->boardFrame.height * 0.115f
        });

    if (AudioMusicIsSilent())
        DrawFontCenteredInRectangle(
            asset->fontCondensed, "MUSIC MUTED",
            (Rectangle){layout->statsPanel.x + layout->statsPanel.width,
                        panelY,
                        layout->controlsX - (layout->statsPanel.x +
                                             layout->statsPanel.width),
                        panelHeight},
            panelHeight * 0.30f, 1.0f,
            (Color){196, 186, 170, 205}, 0);
}
