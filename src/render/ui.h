#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "assets.h"

Rectangle PauseInk(Texture2D texture, float x, float y,
                   float width, float height);

Rectangle PauseIconInk(Texture2D texture, int iconKind);

Rectangle FitInside(Texture2D texture, Rectangle box);
void DrawWhole(Texture2D texture, Rectangle destination);

void DrawInk(Texture2D texture, Rectangle ink, Rectangle destination);

void DrawPauseButtonBase(Texture2D texture, Rectangle destination, Color tint);
void DrawPauseIcon(Texture2D texture, Rectangle button, Color tint,
                   int iconKind);
void DrawPauseMenuButton(Assets *asset, Rectangle button, Texture2D icon,
                         Texture2D label, Rectangle labelInk, int iconKind,
                         bool hovered, bool enabled);

void DrawPaperPlateTinted(Texture2D texture, Rectangle ink,
                          Rectangle destination, Color tint);
void DrawPaperPlate(Texture2D texture, Rectangle ink, Rectangle destination);

void DrawLitTexture(Texture2D texture, Rectangle destination, bool hovered);

void DrawFontCenteredInRectangle(Font font, const char *text, Rectangle area,
                                 float fontSize, float spacing, Color color,
                                 float opticalYOffset);
void Draw_Centered_Text(const char *text, int centerX, int y, int size,
                        Color color);

void UiCursorAndSound(bool overControl);

#endif
