#include "ui.h"

#include "colors.h"
#include "audio.h"

Rectangle PauseInk(Texture2D texture, float x, float y, float width, float height)
{
    return (Rectangle){
        texture.width * x, texture.height * y,
        texture.width * width, texture.height * height
    };
}

Rectangle PauseIconInk(Texture2D texture, int iconKind)
{
    if (iconKind == 0)
        return PauseInk(texture, 0.045f, 0.136f, 0.913f, 0.742f);
    if (iconKind == 1)
        return PauseInk(texture, 0.108f, 0.104f, 0.790f, 0.774f);
    if (iconKind == 3)
        return PauseInk(texture, 0.173f, 0.046f, 0.657f, 0.869f);
    return PauseInk(texture, 0.033f, 0.027f, 0.935f, 0.930f);
}

Rectangle FitInside(Texture2D texture, Rectangle box)
{
    float scale = box.width / texture.width;
    if (box.height / texture.height < scale)
        scale = box.height / texture.height;

    float width = texture.width * scale;
    float height = texture.height * scale;
    return (Rectangle){
        box.x + (box.width - width) / 2.0f,
        box.y + (box.height - height) / 2.0f,
        width, height
    };
}

void DrawWhole(Texture2D texture, Rectangle destination)
{
    Rectangle source = {0, 0, texture.width, texture.height};
    DrawTexturePro(texture, source, destination, (Vector2){0, 0}, 0, WHITE);
}

void DrawInk(Texture2D texture, Rectangle ink, Rectangle destination)
{
    DrawTexturePro(texture, ink, destination, (Vector2){0, 0}, 0, WHITE);
}

void DrawPauseButtonBase(Texture2D texture, Rectangle destination, Color tint)
{
    Rectangle ink = PauseInk(texture, 0.022f, 0.153f, 0.939f, 0.692f);
    float sourceCapWidth = ink.width * 0.26f;
    float destinationCapWidth =
        destination.height * sourceCapWidth / ink.height;

    if (destinationCapWidth * 2 > destination.width)
        destinationCapWidth = destination.width / 2;

    Rectangle leftSource = {ink.x, ink.y, sourceCapWidth, ink.height};
    Rectangle centerSource = {
        ink.x + sourceCapWidth, ink.y,
        ink.width - sourceCapWidth * 2, ink.height
    };
    Rectangle rightSource = {
        ink.x + ink.width - sourceCapWidth, ink.y,
        sourceCapWidth, ink.height
    };

    Rectangle leftDestination = {
        destination.x, destination.y,
        destinationCapWidth, destination.height
    };
    Rectangle centerDestination = {
        destination.x + destinationCapWidth, destination.y,
        destination.width - destinationCapWidth * 2, destination.height
    };
    Rectangle rightDestination = {
        destination.x + destination.width - destinationCapWidth,
        destination.y, destinationCapWidth, destination.height
    };

    DrawTexturePro(texture, leftSource, leftDestination,
                   (Vector2){0, 0}, 0, tint);
    DrawTexturePro(texture, centerSource, centerDestination,
                   (Vector2){0, 0}, 0, tint);
    DrawTexturePro(texture, rightSource, rightDestination,
                   (Vector2){0, 0}, 0, tint);
}

void DrawPauseIcon(Texture2D texture, Rectangle button, Color tint, int iconKind)
{

    float boxSize = button.height * (iconKind == 3 ? 0.80f : 0.62f);
    float centerX = button.x + button.height * 1.20f;
    float centerY = button.y + button.height / 2;

    Rectangle source = PauseIconInk(texture, iconKind);
    float scaleX = boxSize / source.width;
    float scaleY = boxSize / source.height;
    float scale = scaleX < scaleY ? scaleX : scaleY;
    float width = source.width * scale;
    float height = source.height * scale;

    Rectangle destination = {
        centerX - width / 2, centerY - height / 2, width, height
    };
    DrawTexturePro(texture, source, destination, (Vector2){0, 0}, 0, tint);
}

void DrawPauseMenuButton(Assets *asset, Rectangle button, Texture2D icon,
                         Texture2D label, Rectangle labelInk, int iconKind,
                         bool hovered, bool enabled)
{
    Texture2D background = hovered && enabled
        ? asset->texPauseButtonHover
        : asset->texPauseButtonNormal;
    Color tint = WHITE;

    DrawPauseButtonBase(background, button, tint);
    DrawPauseIcon(icon, button, tint, iconKind);

    float labelScale = button.height * 0.33f / labelInk.height;
    float labelWidth = labelInk.width * labelScale;
    float labelHeight = labelInk.height * labelScale;

    float iconRight = button.x + button.height * 1.20f +
                      button.height * 0.31f;
    float labelX = button.x + (button.width - labelWidth) / 2;
    if (labelX < iconRight + button.height * 0.18f)
        labelX = iconRight + button.height * 0.18f;

    Rectangle labelDestination = {
        labelX,
        button.y + (button.height - labelHeight) / 2,
        labelWidth,
        labelHeight
    };
    DrawTexturePro(label, labelInk, labelDestination,
                   (Vector2){0, 0}, 0, tint);
}

void DrawPaperPlateTinted(Texture2D texture, Rectangle ink,
                          Rectangle destination, Color tint)
{
    float capSource = ink.width * 0.18f;
    float capDestination = destination.height * capSource / ink.height;
    if (capDestination * 2.0f > destination.width)
        capDestination = destination.width / 2.0f;

    float plateScale = destination.height / ink.height;
    float midDestWidth = destination.width - capDestination * 2.0f;
    float midSourceAvailable = ink.width - capSource * 2.0f;
    float midSourceWidth = midDestWidth / plateScale;
    if (midSourceWidth > midSourceAvailable)
        midSourceWidth = midSourceAvailable;

    Rectangle leftSource = {ink.x, ink.y, capSource, ink.height};
    Rectangle midSource = {
        ink.x + capSource + (midSourceAvailable - midSourceWidth) / 2.0f,
        ink.y, midSourceWidth, ink.height
    };
    Rectangle rightSource = {ink.x + ink.width - capSource, ink.y,
                             capSource, ink.height};

    Rectangle leftDest = {destination.x, destination.y,
                          capDestination, destination.height};
    Rectangle midDest = {destination.x + capDestination, destination.y,
                         destination.width - capDestination * 2.0f,
                         destination.height};
    Rectangle rightDest = {destination.x + destination.width - capDestination,
                           destination.y, capDestination, destination.height};

    DrawTexturePro(texture, leftSource, leftDest, (Vector2){0, 0}, 0, tint);
    DrawTexturePro(texture, midSource, midDest, (Vector2){0, 0}, 0, tint);
    DrawTexturePro(texture, rightSource, rightDest, (Vector2){0, 0}, 0, tint);
}

void DrawPaperPlate(Texture2D texture, Rectangle ink, Rectangle destination)
{
    DrawPaperPlateTinted(texture, ink, destination, WHITE);
}

void DrawLitTexture(Texture2D texture, Rectangle destination, bool hovered)
{
    DrawWhole(texture, destination);
    if (!hovered)
        return;

    Rectangle source = {0, 0, texture.width, texture.height};
    BeginBlendMode(BLEND_ADDITIVE);
    DrawTexturePro(texture, source, destination, (Vector2){0, 0}, 0,
                   COL_HOVER_LIFT);
    EndBlendMode();
}

void DrawFontCenteredInRectangle(Font font, const char *text, Rectangle area,
                                 float fontSize, float spacing, Color color,
                                 float opticalYOffset)
{
    Vector2 size = MeasureTextEx(font, text, fontSize, spacing);
    Vector2 position = {
        area.x + (area.width - size.x) / 2,
        area.y + (area.height - size.y) / 2 + opticalYOffset
    };
    DrawTextEx(font, text, position, fontSize, spacing, color);
}

void Draw_Centered_Text(const char *text, int centerX, int y, int size, Color color)
{
    int x = centerX - MeasureText(text, size) / 2;
    DrawText(text, x + 3, y + 3, size, Fade(BLACK, 0.8f));
    DrawText(text, x, y, size, color);
}

void UiCursorAndSound(bool overControl)
{
    static bool wasOver = false;

    SetMouseCursor(overControl ? MOUSE_CURSOR_POINTING_HAND
                              : MOUSE_CURSOR_DEFAULT);

    if (overControl && !wasOver)
        AudioPlay(SFX_UI_HOVER);
    if (overControl && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        AudioPlay(SFX_UI_CLICK);

    wasOver = overControl;
}
