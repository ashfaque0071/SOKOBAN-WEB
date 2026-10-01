#ifndef CONTROLS_H
#define CONTROLS_H

#include "raylib.h"
#include "assets.h"

#include "ui.h"

typedef struct ControlsLayout {
    Rectangle screen;
    Rectangle title;
    Rectangle panel;
    Rectangle paper;
    Rectangle backButton;
} ControlsLayout;

typedef enum ControlsControl {
    CONTROLS_CONTROL_NONE = -1,
    CONTROLS_CONTROL_BACK
} ControlsControl;

ControlsLayout ControlsGetLayout(Assets *asset, float screenWidth,
                                 float screenHeight);
ControlsControl ControlsHitTest(const ControlsLayout *layout, Vector2 mouse);
void ControlsDraw(Assets *asset, const ControlsLayout *layout,
                  ControlsControl hovered);

#endif
