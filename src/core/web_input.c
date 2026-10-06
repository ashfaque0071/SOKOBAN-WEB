#include "web_input.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

/* A compact per-frame snapshot keeps browser touch events separate from
   raylib's mouse emulation. The latter can otherwise register the same tap
   twice on some mobile browsers. */
static int touchSnapshot;

void InputBeginFrame(void)
{
#if defined(PLATFORM_WEB)
    touchSnapshot = emscripten_run_script_int(
        "window.sokobanInputSnapshot ? window.sokobanInputSnapshot() : 0");
#endif
}

void InputSetScreen(int screen)
{
#if defined(PLATFORM_WEB)
    static int lastScreen = -1;
    if (screen != lastScreen)
    {
        emscripten_run_script(TextFormat(
            "window.sokobanMobileScreen(%d)", screen));
        lastScreen = screen;
    }
#else
    (void)screen;
#endif
}

static bool TouchSuppressesMouse(void)
{
    return (touchSnapshot & (1 << 24)) != 0;
}

Vector2 InputPointerPosition(void)
{
    if (TouchSuppressesMouse())
        return (Vector2){(float)(touchSnapshot & 2047),
                         (float)((touchSnapshot >> 11) & 1023)};
    return GetMousePosition();
}

bool InputPointerPressed(void)
{
    return TouchSuppressesMouse()
        ? (touchSnapshot & (1 << 21)) != 0
        : IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

bool InputPointerDown(void)
{
    return TouchSuppressesMouse()
        ? (touchSnapshot & (1 << 22)) != 0
        : IsMouseButtonDown(MOUSE_BUTTON_LEFT);
}

bool InputPointerReleased(void)
{
    return TouchSuppressesMouse()
        ? (touchSnapshot & (1 << 23)) != 0
        : IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

bool InputKeyPressed(int key)
{
    return IsKeyPressed(key);
}

int InputDirection(void)
{
    return (touchSnapshot >> 25) & 7;
}

bool InputDirectionPressed(void)
{
    return (touchSnapshot & (1 << 28)) != 0;
}
