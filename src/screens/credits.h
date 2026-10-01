#ifndef CREDITS_H
#define CREDITS_H

#include "raylib.h"
#include "assets.h"

#include "ui.h"

typedef struct CreditsLayout {
    Rectangle screen;
    Rectangle title;
    Rectangle panel;
    Rectangle paper;
    Rectangle backButton;
} CreditsLayout;

typedef enum CreditsControl {
    CREDITS_CONTROL_NONE = -1,
    CREDITS_CONTROL_BACK
} CreditsControl;

CreditsLayout CreditsGetLayout(Assets *asset, float screenWidth,
                               float screenHeight);
CreditsControl CreditsHitTest(const CreditsLayout *layout, Vector2 mouse);
void CreditsDraw(Assets *asset, const CreditsLayout *layout,
                 CreditsControl hovered);

#endif
