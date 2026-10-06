#include "settings.h"

#include "raymath.h"
#include "audio.h"
#include "board.h"
#include "colors.h"
#include "score.h"
#include "web_input.h"
#include "screen.h"
#include "ui.h"

static const float REPEAT_DELAYS[SETTINGS_SPEED_COUNT] = {
    0.35f, 0.25f, 0.15f
};

static const Color SETTINGS_INK = {38, 33, 29, 255};
static const Color SETTINGS_FAINT = {92, 82, 72, 255};

static Rectangle GetConfirmationPanel(Assets *assets);
static Rectangle GetConfirmationButton(Rectangle panel, int buttonIndex);

static Rectangle ScaleSettingsRect(float x, float y, float width, float height)
{
    return (Rectangle){
        x * SCREEN_W / 1672.0f, y * SCREEN_H / 941.0f,
        width * SCREEN_W / 1672.0f, height * SCREEN_H / 941.0f
    };
}

#define SETTINGS_LIFT_ALPHA 30
#define SETTINGS_LIFT COL_HOVER_LIFT_AT(SETTINGS_LIFT_ALPHA)

#define SETTINGS_HOVER_ART (Color){243, 243, 243, 255}
#define CONFIRM_HOVER_ART  (Color){238, 238, 238, 255}

static void DrawPaperPlateHighlight(Texture2D texture, Rectangle source,
                                    Rectangle destination)
{
    BeginBlendMode(BLEND_ADDITIVE);
    DrawPaperPlateTinted(texture, source, destination, SETTINGS_LIFT);
    EndBlendMode();
}

#define SETTINGS_CAP_RATIO 0.56f
#define SETTINGS_TRACKING (-0.030f)
#define SETTINGS_OPTICAL (-0.030f)

static float GetSettingsFontSize(float capHeight)
{
    return capHeight * SCREEN_H / 941.0f / SETTINGS_CAP_RATIO;
}

static void DrawSettingsLabel(Font font, const char *text, float x,
                              float capTop, float capHeight, Color color)
{
    float size = GetSettingsFontSize(capHeight);
    Rectangle box = ScaleSettingsRect(x, capTop, 0, capHeight);
    Vector2 measured = MeasureTextEx(font, text, size, SETTINGS_TRACKING * size);
    DrawTextEx(font, text,
               (Vector2){box.x,
                         box.y + (box.height - measured.y) / 2.0f
                             + size * SETTINGS_OPTICAL},
               size, SETTINGS_TRACKING * size, color);
}

static void DrawSettingsLabelCentered(Font font, const char *text,
                                      float centerX, float capTop,
                                      float capHeight, Color color)
{
    float size = GetSettingsFontSize(capHeight);
    Vector2 measured = MeasureTextEx(font, text, size, SETTINGS_TRACKING * size);
    Rectangle box = ScaleSettingsRect(0, capTop, 0, capHeight);
    DrawTextEx(font, text,
               (Vector2){centerX * SCREEN_W / 1672.0f - measured.x / 2.0f,
                         box.y + (box.height - measured.y) / 2.0f
                             + size * SETTINGS_OPTICAL},
               size, SETTINGS_TRACKING * size, color);
}

static void DrawSettingsLabelInRect(Font font, const char *text,
                                    Rectangle rect, float capHeight,
                                    Color color)
{
    float size = GetSettingsFontSize(capHeight);
    Vector2 measured = MeasureTextEx(font, text, size, SETTINGS_TRACKING * size);
    DrawTextEx(font, text,
               (Vector2){rect.x + (rect.width - measured.x) / 2.0f,
                         rect.y + (rect.height - measured.y) / 2.0f
                             + size * SETTINGS_OPTICAL},
               size, SETTINGS_TRACKING * size, color);
}

SettingsLayout SettingsGetLayout(Assets *asset)
{
    SettingsLayout layout = {0};
    layout.backButton = ScaleSettingsRect(45, 28, 145, 76);
    layout.masterOption[0] = ScaleSettingsRect(844, 294, 151, 56);
    layout.masterOption[1] = ScaleSettingsRect(1000, 294, 157, 56);
    layout.musicSlider = ScaleSettingsRect(734, 362, 438, 34);
    layout.effectsSlider = ScaleSettingsRect(734, 412, 438, 33);
    layout.movementOption[0] = ScaleSettingsRect(747, 515, 243, 57);
    layout.movementOption[1] = ScaleSettingsRect(996, 515, 254, 57);
    layout.speedOption[0] = ScaleSettingsRect(747, 584, 153, 55);
    layout.speedOption[1] = ScaleSettingsRect(910, 584, 179, 55);
    layout.speedOption[2] = ScaleSettingsRect(1097, 584, 153, 55);
    layout.resetButton = ScaleSettingsRect(550, 714, 580, 78);

    layout.confirmationPanel = GetConfirmationPanel(asset);
    layout.confirmationButton[0] =
        GetConfirmationButton(layout.confirmationPanel, 0);
    layout.confirmationButton[1] =
        GetConfirmationButton(layout.confirmationPanel, 1);
    return layout;
}

static float GetSliderInset(Rectangle track)
{
    return track.width * 0.030f;
}

static float GetSliderKnobX(Rectangle track, float value)
{
    float inset = GetSliderInset(track);
    return track.x + inset + value * (track.width - inset * 2.0f);
}

static float GetSliderValue(Rectangle track, float mouseX)
{
    float inset = GetSliderInset(track);
    return Clamp((mouseX - track.x - inset) /
                 (track.width - inset * 2.0f), 0.0f, 1.0f);
}

static void DrawSettingsSlider(Assets *asset, Rectangle track, float value,
                               bool hovered)
{
    Rectangle trackInk = PauseInk(asset->texSetTrackV2,
                                  0.040f, 0.341f, 0.915f, 0.315f);
    DrawPaperPlate(asset->texSetTrackV2, trackInk, track);
    if (hovered)
        DrawPaperPlateHighlight(asset->texSetTrackV2, trackInk, track);

    float inset = GetSliderInset(track);
    float span = track.width - inset * 2.0f;
    if (value > 0.005f)
    {
        Rectangle fillInk = PauseInk(asset->texSetFillV2,
                                     0.030f, 0.384f, 0.939f, 0.222f);
        Rectangle source = {fillInk.x, fillInk.y,
                            fillInk.width * value, fillInk.height};
        Rectangle destination = {track.x + inset,
                                 track.y + track.height * 0.30f,
                                 span * value, track.height * 0.40f};
        DrawTexturePro(asset->texSetFillV2, source, destination,
                       (Vector2){0, 0}, 0, WHITE);
    }

    float knob = track.height * 1.35f;
    DrawInk(asset->texSetKnobV2,
            PauseInk(asset->texSetKnobV2, 0.312f, 0.309f, 0.376f, 0.379f),
            (Rectangle){GetSliderKnobX(track, value) - knob / 2.0f,
                        track.y + track.height / 2.0f - knob / 2.0f,
                        knob, knob});
}

#define SETTINGS_OPTION_CAP 19.0f
#define SETTINGS_OPTION_PIN 0.55f
#define SETTINGS_OPTION_EDGE 0.18f

static void DrawSettingsOption(Assets *asset, Rectangle segment,
                               const char *text, bool selected, bool hovered)
{
    Texture2D plate = selected ? asset->texSetOptSelV2
                               : asset->texSetOptUnselV2;

    Rectangle ink = selected
        ? PauseInk(plate, 0.076f, 0.240f, 0.894f, 0.517f)
        : PauseInk(plate, 0.031f, 0.253f, 0.933f, 0.515f);
    DrawPaperPlate(plate, ink, segment);
    if (hovered)
        DrawPaperPlateHighlight(plate, ink, segment);

    float pin = segment.height * SETTINGS_OPTION_PIN;
    float edge = segment.height * SETTINGS_OPTION_EDGE;
    Rectangle clear = {segment.x + pin, segment.y,
                       segment.width - pin - edge, segment.height};
    DrawSettingsLabelInRect(asset->fontCondensed, text, clear,
                            SETTINGS_OPTION_CAP, SETTINGS_INK);
}

static Rectangle GetConfirmationPanel(Assets *asset)
{
    Rectangle ink = PauseInk(asset->texSetConfirmPanelV2,
                             0.023f, 0.043f, 0.958f, 0.910f);
    float width = SCREEN_W * 0.455f;
    return (Rectangle){(SCREEN_W - width) / 2.0f, SCREEN_H * 0.315f, width,
                       width * ink.height / ink.width};
}

static Rectangle GetConfirmationButton(Rectangle panel, int buttonIndex)
{
    return (Rectangle){
        panel.x + panel.width * (buttonIndex == 0 ? 0.100f : 0.505f),
        panel.y + panel.height * 0.690f,
        panel.width * 0.395f, panel.height * 0.205f
    };
}

static void DrawConfirmationButton(Assets *asset, Rectangle button,
                                   const char *label, bool erase,
                                   bool hovered, bool pressed)
{
    Texture2D plate;
    if (erase)
    {
        if (pressed)
            plate = asset->texSetResetPressedV2;
        else if (hovered)
            plate = asset->texSetResetHoverV2;
        else
            plate = asset->texSetResetNormalV2;
    }
    else
    {
        if (pressed)
            plate = asset->texSetKeepPressed;
        else if (hovered)
            plate = asset->texSetKeepHover;
        else
            plate = asset->texSetKeepNormal;
    }

    Rectangle ink = (hovered && !pressed)
        ? PauseInk(plate, 0.024f, 0.189f, 0.953f, 0.601f)
        : PauseInk(plate, 0.028f, 0.200f, 0.945f, 0.578f);

    DrawPaperPlateTinted(plate, ink, button,
                         (hovered && !pressed) ? CONFIRM_HOVER_ART : WHITE);

    Color word;
    if (erase)
        word = hovered ? (Color){232, 72, 58, 255} : COL_NOIR_RED;
    else
        word = hovered ? SETTINGS_INK : SETTINGS_FAINT;
    DrawSettingsLabelInRect(asset->fontCondensed, label, button, 22.0f, word);
}

static void DrawSettingsScreen(Assets *asset, SettingsLayout layout,
                               bool masterSoundOn, float musicVolume,
                               float effectsVolume, int holdToRepeat,
                               int speedIndex, int resetButtonState,
                               bool showConfirmation, SettingsHover hover)
{
    Rectangle screen = {0, 0, SCREEN_W, SCREEN_H};

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
    DrawTexturePro(asset->texMenuBackground, source, screen,
                   (Vector2){0, 0}, 0, WHITE);

    DrawWhole(asset->texSettingsPanel,
              (Rectangle){SCREEN_W * 0.0642f, SCREEN_H * 0.0597f,
                          SCREEN_W * 0.8416f, SCREEN_H * 0.8992f});
    DrawLitTexture(asset->texLevelsButtonBack, layout.backButton, hover.back);

    Font font = asset->fontCondensed;

    DrawSettingsLabelCentered(font, "SETTINGS", 850.0f, 181.0f, 61.0f,
                              SETTINGS_INK);

    DrawSettingsLabel(font, "AUDIO", 404.0f, 257.0f, 32.0f, SETTINGS_INK);
    DrawSettingsLabel(font, "GAMEPLAY", 405.0f, 488.0f, 32.0f, SETTINGS_INK);
    DrawSettingsLabel(font, "PROGRESS", 405.0f, 675.0f, 32.0f, SETTINGS_INK);

    DrawSettingsLabel(font, "MASTER SOUND", 428.0f, 320.0f, 22.0f,
                      SETTINGS_INK);
    DrawSettingsLabel(font, "MUSIC VOLUME", 428.0f, 370.0f, 22.0f,
                      SETTINGS_INK);
    DrawSettingsLabel(font, "SOUND EFFECTS", 428.0f, 419.0f, 22.0f,
                      SETTINGS_INK);
    DrawSettingsLabel(font, "MOVEMENT MODE", 428.0f, 547.0f, 22.0f,
                      SETTINGS_INK);
    DrawSettingsLabel(font, "HOLD REPEAT SPEED", 428.0f, 601.0f, 22.0f,
                      SETTINGS_INK);

    DrawSettingsOption(asset, layout.masterOption[0], "ON", masterSoundOn,
                       hover.masterOption == 0);
    DrawSettingsOption(asset, layout.masterOption[1], "OFF", !masterSoundOn,
                       hover.masterOption == 1);

    DrawSettingsSlider(asset, layout.musicSlider, musicVolume,
                       hover.musicSlider);
    DrawSettingsSlider(asset, layout.effectsSlider, effectsVolume,
                       hover.effectsSlider);

    DrawSettingsLabel(font, TextFormat("%i%%",
                      (int)(musicVolume * 100.0f + 0.5f)),
                      1188.0f, 367.0f, 24.0f, SETTINGS_INK);
    DrawSettingsLabel(font, TextFormat("%i%%",
                      (int)(effectsVolume * 100.0f + 0.5f)),
                      1188.0f, 417.0f, 24.0f, SETTINGS_INK);

    DrawSettingsOption(asset, layout.movementOption[0], "SINGLE STEP",
                       !holdToRepeat, hover.movementOption == 0);
    DrawSettingsOption(asset, layout.movementOption[1], "HOLD TO REPEAT",
                       holdToRepeat, hover.movementOption == 1);
    DrawSettingsOption(asset, layout.speedOption[0], "SLOW",
                       speedIndex == 0, hover.speedOption == 0);
    DrawSettingsOption(asset, layout.speedOption[1], "NORMAL",
                       speedIndex == 1, hover.speedOption == 1);
    DrawSettingsOption(asset, layout.speedOption[2], "FAST",
                       speedIndex == 2, hover.speedOption == 2);

    Texture2D resetPlate = asset->texSetResetNormalV2;
    if (resetButtonState == 2)
        resetPlate = asset->texSetResetPressedV2;
    else if (resetButtonState == 1)
        resetPlate = asset->texSetResetHoverV2;
    Rectangle resetInk = resetButtonState == 1
        ? PauseInk(resetPlate, 0.024f, 0.189f, 0.953f, 0.601f)
        : PauseInk(resetPlate, 0.028f, 0.200f, 0.945f, 0.578f);

    DrawPaperPlateTinted(resetPlate, resetInk, layout.resetButton,
                         resetButtonState == 1 ? SETTINGS_HOVER_ART : WHITE);
    DrawSettingsLabelCentered(font, "RESET ALL PROGRESS", 841.0f, 741.0f,
                              31.0f, SETTINGS_INK);
    DrawSettingsLabelCentered(font, "ERASE SOLVED CASES, STARS & BEST SCORES",
                              834.0f, 803.0f, 15.0f, SETTINGS_INK);

    if (!showConfirmation)
        return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.58f));
    Rectangle confirmPanel = layout.confirmationPanel;
    DrawInk(asset->texSetConfirmPanelV2,
            PauseInk(asset->texSetConfirmPanelV2,
                     0.023f, 0.043f, 0.958f, 0.910f), confirmPanel);

    DrawSettingsLabelInRect(
        font, "ERASE EVERY CASE?",
        (Rectangle){confirmPanel.x, confirmPanel.y + confirmPanel.height * 0.32f,
                    confirmPanel.width, confirmPanel.height * 0.16f},
        34.0f, SETTINGS_INK);
    DrawSettingsLabelInRect(
        font, "STARS AND BEST SCORES GO WITH THEM",
        (Rectangle){confirmPanel.x, confirmPanel.y + confirmPanel.height * 0.50f,
                    confirmPanel.width, confirmPanel.height * 0.10f},
        15.0f, SETTINGS_FAINT);

    DrawConfirmationButton(asset, layout.confirmationButton[0], "ERASE", true,
                           hover.confirmationOption == 0,
                           hover.confirmationOption == 0 &&
                               hover.confirmationPressed);
    DrawConfirmationButton(asset, layout.confirmationButton[1], "KEEP", false,
                           hover.confirmationOption == 1,
                           hover.confirmationOption == 1 &&
                               hover.confirmationPressed);
}

float SettingsRepeatDelay(int speedIndex)
{
    if (speedIndex < 0 || speedIndex >= SETTINGS_SPEED_COUNT)
        speedIndex = 1;
    return REPEAT_DELAYS[speedIndex];
}

void SettingsDefaults(SettingsState *state)
{
    SettingsState fresh = {0};
    fresh.masterSoundOn = true;
    fresh.musicVolume = 0.0f;
    fresh.effectsVolume = 0.80f;
    fresh.holdToRepeat = 1;
    fresh.speedIndex = 1;
    fresh.origin = SETTINGS_FROM_MENU;
    fresh.hover.masterOption = -1;
    fresh.hover.movementOption = -1;
    fresh.hover.speedOption = -1;
    fresh.hover.confirmationOption = -1;
    *state = fresh;
}

void SettingsApply(const SettingsState *state)
{
    AudioSetMasterOn(state->masterSoundOn);
    AudioSetMusicVolume(state->musicVolume);
    AudioSetEffectsVolume(state->effectsVolume);
    BoardSetMovement(state->holdToRepeat,
                     SettingsRepeatDelay(state->speedIndex));
}

void SettingsDraw(Assets *asset, const SettingsLayout *layout,
                  const SettingsState *state)
{
    DrawSettingsScreen(asset, *layout, state->masterSoundOn,
                       state->musicVolume, state->effectsVolume,
                       state->holdToRepeat, state->speedIndex,
                       state->resetButtonState, state->drawConfirmation,
                       state->hover);
}

SettingsResult SettingsUpdate(const SettingsLayout *layout,
                              SettingsState *state, Vector2 mouse)
{
    bool leftMousePressed = InputPointerPressed();
    bool leftMouseHeld = InputPointerDown();

    bool backHovered = CheckCollisionPointRec(mouse, layout->backButton);
    bool musicSliderHovered = CheckCollisionPointRec(mouse,
                                                     layout->musicSlider);
    bool effectsSliderHovered = CheckCollisionPointRec(mouse,
                                                       layout->effectsSlider);
    bool resetHovered = CheckCollisionPointRec(mouse, layout->resetButton);

    int masterOptionHovered = -1;
    for (int i = 0; i < 2; i++)
    {
        if (CheckCollisionPointRec(mouse, layout->masterOption[i]))
            masterOptionHovered = i;
    }

    int movementOptionHovered = -1;
    for (int i = 0; i < 2; i++)
    {
        if (CheckCollisionPointRec(mouse, layout->movementOption[i]))
            movementOptionHovered = i;
    }

    int speedOptionHovered = -1;
    for (int i = 0; i < SETTINGS_SPEED_COUNT; i++)
    {
        if (CheckCollisionPointRec(mouse, layout->speedOption[i]))
            speedOptionHovered = i;
    }

    int confirmationOptionHovered = -1;
    if (CheckCollisionPointRec(mouse, layout->confirmationButton[0]))
        confirmationOptionHovered = 0;
    else if (CheckCollisionPointRec(mouse, layout->confirmationButton[1]))
        confirmationOptionHovered = 1;

    SettingsHover currentHover = {0};
    currentHover.masterOption = -1;
    currentHover.movementOption = -1;
    currentHover.speedOption = -1;
    currentHover.confirmationOption = -1;

    if (state->showConfirmation)
    {
        SettingsResult result = SETTINGS_STAY;

        if (leftMousePressed && confirmationOptionHovered == 0)
        {
            ScoreResetAll();
            state->showConfirmation = false;
            result = SETTINGS_ERASED;
        }
        else if ((leftMousePressed && confirmationOptionHovered == 1) ||
                 InputKeyPressed(KEY_ESCAPE))
        {
            state->showConfirmation = false;
        }

        UiCursorAndSound(confirmationOptionHovered >= 0);

        currentHover.confirmationOption = confirmationOptionHovered;
        currentHover.confirmationPressed = leftMouseHeld;
        state->hover = currentHover;
        state->resetButtonState = 0;
        state->drawConfirmation = true;
        return result;
    }

    UiCursorAndSound(backHovered || masterOptionHovered >= 0 ||
                     musicSliderHovered || effectsSliderHovered ||
                     resetHovered || movementOptionHovered >= 0 ||
                     speedOptionHovered >= 0);

    if (leftMousePressed && masterOptionHovered >= 0)
    {
        state->masterSoundOn = (masterOptionHovered == 0);
        AudioSetMasterOn(state->masterSoundOn);
    }
    if (leftMousePressed && movementOptionHovered >= 0)
    {
        state->holdToRepeat = movementOptionHovered;
        BoardSetMovement(state->holdToRepeat,
                         SettingsRepeatDelay(state->speedIndex));
    }
    if (leftMousePressed && speedOptionHovered >= 0)
    {
        state->speedIndex = speedOptionHovered;
        BoardSetMovement(state->holdToRepeat,
                         SettingsRepeatDelay(state->speedIndex));
    }
    if (leftMousePressed && resetHovered)
        state->showConfirmation = true;

    if (leftMousePressed && musicSliderHovered)
        state->draggingMusicSlider = true;
    if (leftMousePressed && effectsSliderHovered)
        state->draggingEffectsSlider = true;

    if (leftMouseHeld && state->draggingMusicSlider)
    {
        state->musicVolume = GetSliderValue(layout->musicSlider, mouse.x);
        AudioSetMusicVolume(state->musicVolume);
        AudioUnmute();
    }
    if (leftMouseHeld && state->draggingEffectsSlider)
    {
        state->effectsVolume = GetSliderValue(layout->effectsSlider, mouse.x);
        AudioSetEffectsVolume(state->effectsVolume);
    }
    if (InputPointerReleased())
    {
        state->draggingMusicSlider = false;
        state->draggingEffectsSlider = false;
    }

    bool shouldClose = (leftMousePressed && backHovered) ||
        InputKeyPressed(KEY_ESCAPE);
    if (shouldClose)
    {
        state->draggingMusicSlider = false;
        state->draggingEffectsSlider = false;
    }

    currentHover.back = backHovered;
    currentHover.musicSlider = musicSliderHovered;
    currentHover.effectsSlider = effectsSliderHovered;
    currentHover.masterOption = masterOptionHovered;
    currentHover.movementOption = movementOptionHovered;
    currentHover.speedOption = speedOptionHovered;
    state->hover = currentHover;

    if (!resetHovered)
        state->resetButtonState = 0;
    else if (leftMouseHeld)
        state->resetButtonState = 2;
    else
        state->resetButtonState = 1;

    state->drawConfirmation = false;

    return shouldClose ? SETTINGS_CLOSED : SETTINGS_STAY;
}
