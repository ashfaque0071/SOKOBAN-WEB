#include "pause.h"

#include "colors.h"
#include "screen.h"
#include "ui.h"

PauseLayout PauseGetLayout(Assets *asset)
{
    PauseLayout layout = {0};

    float panelHeight = SCREEN_H * 0.80f;
    float panelWidth = panelHeight *
        asset->texPausePanel.width / asset->texPausePanel.height;
    layout.panel = (Rectangle){
        (SCREEN_W - panelWidth) / 2,
        (SCREEN_H - panelHeight) / 2,
        panelWidth,
        panelHeight
    };

    float buttonWidth = layout.panel.width * 0.65f;
    float buttonHeight = layout.panel.height * 0.093f;
    float buttonGap = layout.panel.height * 0.020f;
    float buttonX = layout.panel.x + (layout.panel.width - buttonWidth) / 2;
    float buttonY = layout.panel.y + layout.panel.height * 0.375f;

    layout.resumeButton = (Rectangle){
        buttonX, buttonY, buttonWidth, buttonHeight
    };
    layout.restartButton = (Rectangle){
        buttonX, buttonY + buttonHeight + buttonGap,
        buttonWidth, buttonHeight
    };
    layout.settingsButton = (Rectangle){
        buttonX, buttonY + (buttonHeight + buttonGap) * 2,
        buttonWidth, buttonHeight
    };
    layout.homeButton = (Rectangle){
        buttonX, buttonY + (buttonHeight + buttonGap) * 3,
        buttonWidth, buttonHeight
    };
    return layout;
}

static void DrawPauseStatus(Font font, const Board *board,
                            Rectangle area, float fontSize, float spacing,
                            Color color)
{
    const char *levelText = TextFormat("LEVEL %i", board->currentLevel + 1);
    char level[32];
    TextCopy(level, levelText);

    const char *movesText = TextFormat("%i MOVES", board->moveCount);
    char moves[32];
    TextCopy(moves, movesText);

    const char *pushesText = TextFormat("%i PUSHES", board->pushCount);
    char pushes[32];
    TextCopy(pushes, pushesText);

    Vector2 levelSize = MeasureTextEx(font, level, fontSize, spacing);
    Vector2 movesSize = MeasureTextEx(font, moves, fontSize, spacing);
    Vector2 pushesSize = MeasureTextEx(font, pushes, fontSize, spacing);
    float separatorSpace = fontSize * 0.72f;
    float totalWidth = levelSize.x + movesSize.x + pushesSize.x +
                       separatorSpace * 2;
    float x = area.x + (area.width - totalWidth) / 2.0f;
    float y = area.y + (area.height - levelSize.y) / 2.0f - 1.0f;

    DrawTextEx(font, level, (Vector2){x, y}, fontSize, spacing, color);
    x += levelSize.x;
    DrawCircleV((Vector2){x + separatorSpace / 2.0f,
                          y + levelSize.y * 0.53f},
                fontSize * 0.075f, color);
    x += separatorSpace;

    DrawTextEx(font, moves, (Vector2){x, y}, fontSize, spacing, color);
    x += movesSize.x;
    DrawCircleV((Vector2){x + separatorSpace / 2.0f, y + movesSize.y * 0.53f},
                fontSize * 0.075f, color);
    x += separatorSpace;

    DrawTextEx(font, pushes, (Vector2){x, y}, fontSize, spacing, color);
}

void PauseDraw(const Board *board, Assets *asset, const PauseLayout *layout,
               bool resumeHovered, bool restartHovered,
               bool settingsHovered, bool homeHovered)
{
    Rectangle panel = layout->panel;

    Rectangle panelSource = {
        0, 0, asset->texPausePanel.width, asset->texPausePanel.height
    };
    DrawTexturePro(asset->texPausePanel, panelSource, panel,
                   (Vector2){0, 0}, 0, WHITE);

    float centerX = panel.x + panel.width / 2;

    Rectangle titleInk = PauseInk(asset->texPauseTextTitle,
                                  0.005f, 0.025f, 0.990f, 0.950f);
    float titleWidth = panel.width * 0.56f;
    float titleHeight = titleWidth * titleInk.height / titleInk.width;
    float titleY = panel.y + panel.height * 0.170f;
    Rectangle titleDestination = {
        centerX - titleWidth / 2, titleY, titleWidth, titleHeight
    };
    DrawTexturePro(asset->texPauseTextTitle, titleInk, titleDestination,
                   (Vector2){0, 0}, 0, WHITE);

    float ruleY = titleY + titleHeight + panel.height * 0.028f;
    DrawLineEx((Vector2){centerX - panel.width * 0.27f, ruleY},
               (Vector2){centerX + panel.width * 0.27f, ruleY},
               panel.height * 0.0045f, COL_NOIR_RED);

    Rectangle statusArea = {
        panel.x + panel.width * 0.12f,
        ruleY + panel.height * 0.016f,
        panel.width * 0.76f,
        panel.height * 0.046f
    };
    DrawPauseStatus(asset->fontNoir, board, statusArea,
                    panel.height * 0.031f, -0.6f,
                    (Color){38, 33, 29, 255});

    Rectangle resumeInk = PauseInk(asset->texPauseTextResume,
                                   0.116f, 0.215f, 0.665f, 0.446f);
    Rectangle restartInk = PauseInk(asset->texPauseTextRestart,
                                    0.073f, 0.229f, 0.812f, 0.429f);
    Rectangle settingsInk = PauseInk(asset->texPauseTextSettings,
                                     0.100f, 0.229f, 0.678f, 0.429f);
    Rectangle homeInk = PauseInk(asset->texPauseTextHome,
                                 0.082f, 0.271f, 0.714f, 0.414f);

    DrawPauseMenuButton(asset, layout->resumeButton,
                        asset->texPauseIconContinue,
                        asset->texPauseTextResume, resumeInk, 0,
                        resumeHovered, true);
    DrawPauseMenuButton(asset, layout->restartButton,
                        asset->texPauseIconRestart,
                        asset->texPauseTextRestart, restartInk, 1,
                        restartHovered, true);
    DrawPauseMenuButton(asset, layout->settingsButton,
                        asset->texPauseIconSettings,
                        asset->texPauseTextSettings, settingsInk, 2,
                        settingsHovered, true);
    DrawPauseMenuButton(asset, layout->homeButton, asset->texPauseIconHome,
                        asset->texPauseTextHome, homeInk, 3,
                        homeHovered, true);

    Rectangle stampInk = PauseInk(asset->texProgressSavedStamp,
                                  0.056f, 0.246f, 0.889f, 0.486f);
    float stampWidth = panel.width * 0.44f;
    float stampHeight = stampWidth * stampInk.height / stampInk.width;
    Rectangle stampDestination = {
        centerX - stampWidth / 2,
        panel.y + panel.height * 0.878f - stampHeight / 2,
        stampWidth,
        stampHeight
    };
    DrawTexturePro(asset->texProgressSavedStamp,
                   stampInk, stampDestination,
                   (Vector2){0, 0}, 0,
                   (Color){185, 185, 185, 255});
}
