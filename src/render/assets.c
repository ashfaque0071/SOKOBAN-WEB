#include "raylib.h"
#include "assets.h"

Texture2D LoadTile(const char *fileName)
{
    Texture2D image = LoadTexture(fileName);
    if (image.id != 0)
    {
        GenTextureMipmaps(&image);
        SetTextureFilter(image, TEXTURE_FILTER_TRILINEAR);
    }
    return image;
}

void AssetsLoad(Assets *asset)
{
    asset->texPauseButtonNormal = LoadTile("assets/shared/button_normal.png");
    asset->texPauseButtonHover = LoadTile("assets/shared/button_hover.png");
    asset->texPauseIconContinue = LoadTile("assets/shared/icon_continue.png");
    asset->texPauseIconSettings = LoadTile("assets/shared/icon_settings.png");
    asset->texMenuBackground = LoadTile("assets/menu/menu_background.png");
    asset->texMenuTitle = LoadTile("assets/menu/title_logo.png");
    asset->texMenuPanel = LoadTile("assets/menu/menu_panel_large.png");
    asset->texMenuTextContinue = LoadTile("assets/menu/menu_text_continue.png");
    asset->texMenuTextLevelSelect = LoadTile("assets/menu/menu_text_level_select.png");
    asset->texMenuTextSettings = LoadTile("assets/menu/menu_text_settings.png");
    asset->texMenuTextQuit = LoadTile("assets/menu/menu_text_quit.png");
    asset->fontNoir = LoadFontEx("assets/fonts/Rye-Regular.ttf", 96, 0, 0);
    SetTextureFilter(asset->fontNoir.texture, TEXTURE_FILTER_BILINEAR);
    asset->fontCondensed = LoadFontEx("assets/fonts/Oswald-Bold.ttf", 96, 0, 0);
    SetTextureFilter(asset->fontCondensed.texture, TEXTURE_FILTER_BILINEAR);
#if !defined(PLATFORM_WEB)
    AssetsLoadDeferred(asset);
#endif
}

void AssetsLoadDeferred(Assets *asset)
{
    asset->texWall = LoadTile("assets/gameplay/tiles/wall_brick.png");
    asset->texWallCap = LoadTile("assets/gameplay/tiles/wall_cap.png");
    asset->texFloorA = LoadTile("assets/gameplay/tiles/floor_cobble_a.png");
    asset->texFloorB = LoadTile("assets/gameplay/tiles/floor_wood_a.png");
    asset->texTarget = LoadTile("assets/gameplay/tiles/goal_web_on.png");
    asset->texBox = LoadTile("assets/gameplay/tiles/crate_default.png");
    asset->texBoxDone = LoadTile("assets/gameplay/tiles/crate_locked.png");
    asset->texPlayerUp = LoadTile("assets/gameplay/character/char_idle_up.png");
    asset->texPlayerDown = LoadTile("assets/gameplay/character/char_idle_down.png");
    asset->texPlayerLeft = LoadTile("assets/gameplay/character/char_idle_left.png");
    asset->texPlayerRight = LoadTile("assets/gameplay/character/char_idle_right.png");
    asset->texPlayerPushUp = LoadTile("assets/gameplay/character/char_push_up.png");
    asset->texPlayerPushDown = LoadTile("assets/gameplay/character/char_push_down.png");
    asset->texPlayerPushLeft = LoadTile("assets/gameplay/character/char_push_left.png");
    asset->texPlayerPushRight = LoadTile("assets/gameplay/character/char_push_right.png");
    asset->texHudPanel = LoadTile("assets/gameplay/ui/ui_hud_panel_noir.png");
    asset->texUiButton = LoadTile("assets/gameplay/ui/ui_button_noir.png");
    asset->texUiIcons = LoadTile("assets/gameplay/ui/ui_icons.png");
    asset->texPausePanel = LoadTile("assets/pause/pause_panel.png");
    asset->texPauseIconRestart = LoadTile("assets/shared/icon_restart.png");
    asset->texPauseIconHome = LoadTile("assets/shared/icon_home.png");
    asset->texPauseTextTitle = LoadTile("assets/pause/pause_text_case_paused.png");
    asset->texPauseTextResume = LoadTile("assets/pause/pause_text_resume.png");
    asset->texPauseTextRestart = LoadTile("assets/pause/pause_text_restart_level.png");
    asset->texPauseTextSettings = LoadTile("assets/pause/pause_text_settings.png");
    asset->texPauseTextHome = LoadTile("assets/pause/pause_text_main_menu.png");
    asset->texProgressSavedStamp = LoadTile("assets/pause/progress_saved_stamp.png");
    asset->texCompletion = LoadTile("assets/completion/completion_noir.png");
    asset->texCompletionEdgeOverlay = LoadTile("assets/completion/edge_collage_overlay.png");
    asset->texCompletionTitle = LoadTile("assets/completion/case_closed_banner.png");
    asset->texCompletionNewBest = LoadTile("assets/completion/new_best_stamp.png");
    asset->texCompletionNextLabel = LoadTile("assets/completion/next_level_label.png");
    asset->texCompletionReplayLabel = LoadTile("assets/completion/replay_label.png");
    asset->texStarFilled = LoadTile("assets/completion/star_filled.png");
    asset->texStarEmpty = LoadTile("assets/completion/star_empty.png");
    asset->texGameBackground = LoadTile("assets/gameplay/screen_main_menu.png");
    asset->texLevelsBackground = LoadTile("assets/levels/screen_level_select_decorated.png");
    asset->texLevelsBanner = LoadTile("assets/levels/case_files_banner.png");
    asset->texLevelsCardLocked = LoadTile("assets/levels/card_locked.png");
    asset->texLevelsStarTag = LoadTile("assets/levels/star_tag.png");
    asset->texLevelsPadlock = LoadTile("assets/levels/icon_padlock.png");
    asset->texLevelsStarGold = LoadTile("assets/levels/star_gold.png");
    asset->texLevelsStarGrey = LoadTile("assets/levels/star_grey.png");
    asset->texLevelsStampProgress = LoadTile("assets/levels/stamp_in_progress.png");
    asset->texLevelsButtonBack = LoadTile("assets/levels/button_back.png");
    asset->texLevelsButtonControls = LoadTile("assets/levels/button_controls.png");
    asset->texLevelsIconMap = LoadTile("assets/levels/icon_map.png");
    asset->texCheatBackground = LoadTile("assets/cheatsheet/cheatsheet_background.png");
    asset->texCheatNote = LoadTile("assets/cheatsheet/instruction_note.png");
    asset->texCheatTab = LoadTile("assets/cheatsheet/navigation_tab.png");
    asset->texCheatArrow = LoadTile("assets/cheatsheet/direction_arrow_up.png");
    asset->texCheatPanel = LoadTile("assets/cheatsheet/panel_cheatsheet.png");
    asset->texCheatLocked = LoadTile("assets/cheatsheet/overlay_cheatsheet_locked.png");
    asset->texHighScoresPanel = LoadTile("assets/high_scores/panel_high_scores.png");
    asset->texSettingsPanel = LoadTile("assets/settings/settings_panel.png");
    asset->texSetOptSelV2 = LoadTile("assets/settings/option_selected_v2.png");
    asset->texSetOptUnselV2 = LoadTile("assets/settings/option_unselected_v2.png");
    asset->texSetKnobV2 = LoadTile("assets/settings/control_knob_v2.png");
    asset->texSetTrackV2 = LoadTile("assets/settings/slider_track_v2.png");
    asset->texSetFillV2 = LoadTile("assets/settings/slider_fill_v2.png");
    asset->texSetResetNormalV2 = LoadTile("assets/settings/reset_button_normal_v2.png");
    asset->texSetResetHoverV2 = LoadTile("assets/settings/reset_button_hover_v2.png");
    asset->texSetResetPressedV2 = LoadTile("assets/settings/reset_button_pressed_v2.png");
    asset->texSetKeepNormal = LoadTile("assets/settings/confirm_keep_normal.png");
    asset->texSetKeepHover = LoadTile("assets/settings/confirm_keep_hover.png");
    asset->texSetKeepPressed = LoadTile("assets/settings/confirm_keep_pressed.png");
    asset->texSetConfirmPanelV2 = LoadTile("assets/settings/reset_confirmation_panel_v2.png");

    asset->fontSlab = LoadFontEx("assets/fonts/RobotoSlab-ExtraBold.ttf", 120, 0, 0);
    SetTextureFilter(asset->fontSlab.texture, TEXTURE_FILTER_BILINEAR);
#if defined(PLATFORM_WEB)
    /* WebGL speaks GLSL ES 1.00, not the desktop GLSL 3.30 of blur.fs. */
    asset->shaderBlur = LoadShader(0, "assets/shaders/blur_es.fs");
#else
    asset->shaderBlur = LoadShader(0, "assets/shaders/blur.fs");
#endif
}

void AssetsUnload(Assets *asset)
{
    UnloadTexture(asset->texWall);
    UnloadTexture(asset->texWallCap);
    UnloadTexture(asset->texFloorA);
    UnloadTexture(asset->texFloorB);
    UnloadTexture(asset->texTarget);
    UnloadTexture(asset->texBox);
    UnloadTexture(asset->texBoxDone);
    UnloadTexture(asset->texPlayerUp);
    UnloadTexture(asset->texPlayerDown);
    UnloadTexture(asset->texPlayerLeft);
    UnloadTexture(asset->texPlayerRight);
    UnloadTexture(asset->texPlayerPushUp);
    UnloadTexture(asset->texPlayerPushDown);
    UnloadTexture(asset->texPlayerPushLeft);
    UnloadTexture(asset->texPlayerPushRight);
    UnloadTexture(asset->texHudPanel);
    UnloadTexture(asset->texUiButton);
    UnloadTexture(asset->texUiIcons);
    UnloadTexture(asset->texPausePanel);
    UnloadTexture(asset->texPauseButtonNormal);
    UnloadTexture(asset->texPauseButtonHover);
    UnloadTexture(asset->texPauseIconContinue);
    UnloadTexture(asset->texPauseIconRestart);
    UnloadTexture(asset->texPauseIconSettings);
    UnloadTexture(asset->texPauseIconHome);
    UnloadTexture(asset->texPauseTextTitle);
    UnloadTexture(asset->texPauseTextResume);
    UnloadTexture(asset->texPauseTextRestart);
    UnloadTexture(asset->texPauseTextSettings);
    UnloadTexture(asset->texPauseTextHome);
    UnloadTexture(asset->texProgressSavedStamp);
    UnloadTexture(asset->texCompletion);
    UnloadTexture(asset->texCompletionEdgeOverlay);
    UnloadTexture(asset->texCompletionTitle);
    UnloadTexture(asset->texCompletionNewBest);
    UnloadTexture(asset->texCompletionNextLabel);
    UnloadTexture(asset->texCompletionReplayLabel);
    UnloadTexture(asset->texStarFilled);
    UnloadTexture(asset->texStarEmpty);
    UnloadTexture(asset->texGameBackground);
    UnloadTexture(asset->texMenuBackground);
    UnloadTexture(asset->texMenuTitle);
    UnloadTexture(asset->texMenuPanel);
    UnloadTexture(asset->texMenuTextContinue);
    UnloadTexture(asset->texMenuTextLevelSelect);
    UnloadTexture(asset->texMenuTextSettings);
    UnloadTexture(asset->texMenuTextQuit);
    UnloadTexture(asset->texLevelsBackground);
    UnloadTexture(asset->texLevelsBanner);
    UnloadTexture(asset->texLevelsCardLocked);
    UnloadTexture(asset->texLevelsStarTag);
    UnloadTexture(asset->texLevelsPadlock);
    UnloadTexture(asset->texLevelsStarGold);
    UnloadTexture(asset->texLevelsStarGrey);
    UnloadTexture(asset->texLevelsStampProgress);
    UnloadTexture(asset->texLevelsButtonBack);
    UnloadTexture(asset->texLevelsButtonControls);
    UnloadTexture(asset->texLevelsIconMap);
    UnloadTexture(asset->texCheatBackground);
    UnloadTexture(asset->texCheatNote);
    UnloadTexture(asset->texCheatTab);
    UnloadTexture(asset->texCheatArrow);
    UnloadTexture(asset->texCheatPanel);
    UnloadTexture(asset->texCheatLocked);
    UnloadTexture(asset->texHighScoresPanel);
    UnloadTexture(asset->texSettingsPanel);
    UnloadTexture(asset->texSetOptSelV2);
    UnloadTexture(asset->texSetOptUnselV2);
    UnloadTexture(asset->texSetKnobV2);
    UnloadTexture(asset->texSetTrackV2);
    UnloadTexture(asset->texSetFillV2);
    UnloadTexture(asset->texSetResetNormalV2);
    UnloadTexture(asset->texSetResetHoverV2);
    UnloadTexture(asset->texSetResetPressedV2);
    UnloadTexture(asset->texSetKeepNormal);
    UnloadTexture(asset->texSetKeepHover);
    UnloadTexture(asset->texSetKeepPressed);
    UnloadTexture(asset->texSetConfirmPanelV2);
    UnloadFont(asset->fontNoir);
    UnloadFont(asset->fontCondensed);
    UnloadFont(asset->fontSlab);
    UnloadShader(asset->shaderBlur);
}
