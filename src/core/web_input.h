#ifndef WEB_INPUT_H
#define WEB_INPUT_H

#include <stdbool.h>
#include "raylib.h"

enum {
    INPUT_MENU = 0, INPUT_LEVELS, INPUT_SETTINGS, INPUT_SCORES,
    INPUT_CHEATS, INPUT_CONTROLS, INPUT_CREDITS, INPUT_PLAY,
    INPUT_PAUSED, INPUT_SOLVED
};

void InputBeginFrame(void);
void InputSetScreen(int screen);
Vector2 InputPointerPosition(void);
bool InputPointerPressed(void);
bool InputPointerDown(void);
bool InputPointerReleased(void);
bool InputKeyPressed(int key);
int InputDirection(void);
bool InputDirectionPressed(void);

#endif
