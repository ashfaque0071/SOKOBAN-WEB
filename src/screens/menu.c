#include "menu.h"

#include "colors.h"
#include "screen.h"
#include "ui.h"

static void DrawMenuButton(Assets *asset, Rectangle button, Texture2D label,
                           Texture2D icon, int iconKind, bool hasIcon,
                           bool hovered)
{
    Texture2D background = hovered
        ? asset->texPauseButtonHover
        : asset->texPauseButtonNormal;
    DrawPauseButtonBase(background, button, WHITE);

    if (hasIcon)
    {
        Rectangle source = PauseIconInk(icon, iconKind);
        float box = button.height * 0.62f;
        float scale = box / source.width;
        if (box / source.height < scale)
            scale = box / source.height;

        Rectangle destination = {
            button.x + button.height * 0.892f - source.width * scale / 2.0f,
            button.y + button.height / 2.0f - source.height * scale / 2.0f,
            source.width * scale, source.height * scale
        };
        DrawTexturePro(icon, source, destination, (Vector2){0, 0}, 0, WHITE);
    }

    float labelHeight = button.height * 0.708f;
    float labelWidth = labelHeight * label.width / label.height;
    Rectangle labelSource = {0, 0, label.width, label.height};
    Rectangle labelDestination = {
        button.x + button.width * 0.245f,
        button.y + button.height / 2.0f - labelHeight * 0.446f,
        labelWidth, labelHeight
    };
    DrawTexturePro(label, labelSource, labelDestination,
                   (Vector2){0, 0}, 0, WHITE);
}

MenuLayout MenuGetLayout(Assets *asset)
{
    MenuLayout layout = {0};

    Rectangle titleInk = PauseInk(asset->texMenuTitle,
                                  0.022f, 0.115f, 0.956f, 0.729f);
    float titleWidth = SCREEN_W * 0.4635f;
    layout.title = (Rectangle){
        SCREEN_W * 0.1107f, SCREEN_H * 0.069f,
        titleWidth, titleWidth * titleInk.height / titleInk.width
    };

    layout.panel = (Rectangle){
        SCREEN_W * 0.1136f, SCREEN_H * 0.292f,
        SCREEN_W * 0.4485f, SCREEN_H * 0.510f
    };

    float buttonWidth = layout.panel.width * 0.620f;
    float buttonHeight = layout.panel.height * 0.135f;
    float buttonX = layout.panel.x + layout.panel.width * 0.187f;
    for (int i = 0; i < MENU_ITEMS; i++)
    {
        layout.button[i] = (Rectangle){
            buttonX,
            layout.panel.y + layout.panel.height * (0.173f + 0.175f * i),
            buttonWidth, buttonHeight
        };
    }

    layout.tag = (Rectangle){
        SCREEN_W * 0.060f, SCREEN_H * 0.855f,
        SCREEN_W * 0.245f, SCREEN_H * 0.085f
    };

    float creditsWidth = SCREEN_W * 0.158f;
    layout.credits = (Rectangle){
        SCREEN_W * 0.940f - creditsWidth, layout.tag.y,
        creditsWidth, layout.tag.height
    };
    return layout;
}

int MenuHitTest(const MenuLayout *layout, Vector2 mouse)
{
    for (int i = 0; i < MENU_ITEMS; i++)
    {
        if (CheckCollisionPointRec(mouse, layout->button[i]))
            return i;
    }
    if (CheckCollisionPointRec(mouse, layout->credits))
        return MENU_ITEM_CREDITS;
    return -1;
}

void MenuDraw(const Board *board, Assets *asset, const MenuLayout *layout,
              int hovered)
{

    float scaleX = (float)SCREEN_W / asset->texMenuBackground.width;
    float scaleY = (float)SCREEN_H / asset->texMenuBackground.height;
    float scale = scaleX > scaleY ? scaleX : scaleY;
    float sourceWidth = SCREEN_W / scale;
    float sourceHeight = SCREEN_H / scale;

    Rectangle source = {
        (asset->texMenuBackground.width - sourceWidth) / 2.0f,
        (asset->texMenuBackground.height - sourceHeight) / 2.0f,
        sourceWidth, sourceHeight
    };
    Rectangle screen = {0, 0, SCREEN_W, SCREEN_H};
    DrawTexturePro(asset->texMenuBackground, source, screen,
                   (Vector2){0, 0}, 0, WHITE);

    Rectangle titleInk = PauseInk(asset->texMenuTitle,
                                  0.022f, 0.115f, 0.956f, 0.729f);
    DrawTexturePro(asset->texMenuTitle, titleInk, layout->title,
                   (Vector2){0, 0}, 0, WHITE);

    Rectangle panelSource = {
        0, 0, asset->texMenuPanel.width, asset->texMenuPanel.height
    };
    DrawTexturePro(asset->texMenuPanel, panelSource, layout->panel,
                   (Vector2){0, 0}, 0, WHITE);

    DrawMenuButton(asset, layout->button[0], asset->texMenuTextContinue,
                   asset->texPauseIconContinue, 0, true, hovered == 0);
    DrawMenuButton(asset, layout->button[1], asset->texMenuTextLevelSelect,
                   asset->texPauseIconContinue, 0, false, hovered == 1);
    DrawMenuButton(asset, layout->button[2], asset->texMenuTextSettings,
                   asset->texPauseIconSettings, 2, true, hovered == 2);
    DrawMenuButton(asset, layout->button[3], asset->texMenuTextQuit,
                   asset->texPauseIconContinue, 0, false, hovered == 3);

    DrawPauseButtonBase(asset->texPauseButtonNormal, layout->tag, WHITE);
    DrawFontCenteredInRectangle(
        asset->fontNoir,
        TextFormat("LAST CASE: LEVEL %i - %i PUSHES",
                   board->currentLevel + 1, board->pushCount),
        layout->tag, layout->tag.height * 0.28f, 0.5f,
        (Color){35, 31, 28, 255}, 0
    );

    bool creditsHovered = hovered == MENU_ITEM_CREDITS;
    Texture2D creditsPlate = creditsHovered
        ? asset->texPauseButtonHover
        : asset->texPauseButtonNormal;
    DrawPauseButtonBase(creditsPlate, layout->credits, WHITE);
    if (creditsHovered)
    {
        BeginBlendMode(BLEND_ADDITIVE);
        DrawPauseButtonBase(creditsPlate, layout->credits, COL_HOVER_LIFT);
        EndBlendMode();
    }
    DrawFontCenteredInRectangle(
        asset->fontNoir, "CREDITS", layout->credits,
        layout->credits.height * 0.30f, 0.5f, (Color){35, 31, 28, 255}, 0
    );
}
