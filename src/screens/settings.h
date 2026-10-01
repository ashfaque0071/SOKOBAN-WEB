#ifndef SETTINGS_H
#define SETTINGS_H

#include "raylib.h"
#include "assets.h"

#define SETTINGS_SPEED_COUNT 3

typedef struct SettingsLayout {
    Rectangle backButton;
    Rectangle masterOption[2];
    Rectangle musicSlider;
    Rectangle effectsSlider;
    Rectangle movementOption[2];
    Rectangle speedOption[SETTINGS_SPEED_COUNT];
    Rectangle resetButton;

    Rectangle confirmationPanel;
    Rectangle confirmationButton[2];
} SettingsLayout;

typedef struct SettingsHover {
    bool back;
    bool musicSlider;
    bool effectsSlider;
    int masterOption;
    int movementOption;
    int speedOption;
    int confirmationOption;
    bool confirmationPressed;
} SettingsHover;

typedef enum SettingsOrigin {
    SETTINGS_FROM_MENU,
    SETTINGS_FROM_PAUSE
} SettingsOrigin;

typedef struct SettingsState {
    bool masterSoundOn;
    float musicVolume;
    float effectsVolume;
    int holdToRepeat;
    int speedIndex;
    SettingsOrigin origin;

    bool showConfirmation;
    bool draggingMusicSlider;
    bool draggingEffectsSlider;

    SettingsHover hover;
    int resetButtonState;
    bool drawConfirmation;
} SettingsState;

typedef enum SettingsResult {
    SETTINGS_STAY,
    SETTINGS_CLOSED,
    SETTINGS_ERASED
} SettingsResult;

SettingsLayout SettingsGetLayout(Assets *asset);

float SettingsRepeatDelay(int speedIndex);
void SettingsDefaults(SettingsState *state);

void SettingsApply(const SettingsState *state);

SettingsResult SettingsUpdate(const SettingsLayout *layout,
                              SettingsState *state, Vector2 mouse);
void SettingsDraw(Assets *asset, const SettingsLayout *layout,
                  const SettingsState *state);

#endif
