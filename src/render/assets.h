#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"

typedef struct Assets {
    Texture2D texWall;
    Texture2D texWallCap;
    Texture2D texFloorA;
    Texture2D texFloorB;
    Texture2D texTarget;
    Texture2D texBox;
    Texture2D texBoxDone;

    Texture2D texPlayerUp;
    Texture2D texPlayerDown;
    Texture2D texPlayerLeft;
    Texture2D texPlayerRight;

    Texture2D texPlayerPushUp;
    Texture2D texPlayerPushDown;
    Texture2D texPlayerPushLeft;
    Texture2D texPlayerPushRight;

    Texture2D texHudPanel;
    Texture2D texUiButton;
    Texture2D texUiIcons;
    Texture2D texPausePanel;
    Texture2D texPauseButtonNormal;
    Texture2D texPauseButtonHover;
    Texture2D texPauseIconContinue;
    Texture2D texPauseIconRestart;
    Texture2D texPauseIconSettings;
    Texture2D texPauseIconHome;
    Texture2D texPauseTextTitle;
    Texture2D texPauseTextResume;
    Texture2D texPauseTextRestart;
    Texture2D texPauseTextSettings;
    Texture2D texPauseTextHome;
    Texture2D texProgressSavedStamp;
    Texture2D texCompletion;
    Texture2D texCompletionEdgeOverlay;
    Texture2D texCompletionTitle;
    Texture2D texCompletionNewBest;
    Texture2D texCompletionNextLabel;
    Texture2D texCompletionReplayLabel;
    Texture2D texStarFilled;
    Texture2D texStarEmpty;
    Texture2D texGameBackground;

    Texture2D texMenuBackground;
    Texture2D texMenuTitle;
    Texture2D texMenuPanel;
    Texture2D texMenuTextContinue;
    Texture2D texMenuTextLevelSelect;
    Texture2D texMenuTextSettings;
    Texture2D texMenuTextQuit;

    Texture2D texLevelsBackground;
    Texture2D texLevelsBanner;
    Texture2D texLevelsCardLocked;
    Texture2D texLevelsStarTag;
    Texture2D texLevelsPadlock;
    Texture2D texLevelsStarGold;
    Texture2D texLevelsStarGrey;
    Texture2D texLevelsStampProgress;
    Texture2D texLevelsButtonBack;
    Texture2D texLevelsButtonControls;
    Texture2D texLevelsIconMap;

    Texture2D texCheatBackground;
    Texture2D texCheatNote;
    Texture2D texCheatTab;
    Texture2D texCheatArrow;
    Texture2D texCheatPanel;
    Texture2D texCheatLocked;

    Texture2D texHighScoresPanel;

    Texture2D texSettingsPanel;
    Texture2D texSetOptSelV2;
    Texture2D texSetOptUnselV2;
    Texture2D texSetKnobV2;
    Texture2D texSetTrackV2;
    Texture2D texSetFillV2;
    Texture2D texSetResetNormalV2;
    Texture2D texSetResetHoverV2;
    Texture2D texSetResetPressedV2;

    Texture2D texSetKeepNormal;
    Texture2D texSetKeepHover;
    Texture2D texSetKeepPressed;
    Texture2D texSetConfirmPanelV2;

    Font fontNoir;
    Font fontCondensed;
    Font fontSlab;
    Shader shaderBlur;

} Assets;

void AssetsLoad(Assets *assets);
void AssetsUnload(Assets *assets);

#endif
