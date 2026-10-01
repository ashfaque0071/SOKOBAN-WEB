#include "raylib.h"

#include "assets.h"
#include "audio.h"
#include "board.h"
#include "cheatsheet.h"
#include "colors.h"
#include "completion.h"
#include "controls.h"
#include "credits.h"
#include "highscores.h"
#include "hint.h"
#include "hud.h"
#include "levelselect.h"
#include "menu.h"
#include "pause.h"
#include "saves.h"
#include "score.h"
#include "screen.h"
#include "settings.h"
#include "ui.h"

#define DEV_LEVEL_SKIP 1

static void LoadLevel(Board *board, int levelIndex)
{
    AudioStop(SFX_LEVEL_COMPLETE);
    BoardLoadLevel(board, (levelIndex + LEVEL_COUNT) % LEVEL_COUNT);
    AudioRestartMusic();
}

int main(void)
{
    InitWindow(SCREEN_W, SCREEN_H, "Sokoban");
    AudioInit();
    SetTargetFPS(60);
    Assets assets;
    AssetsLoad(&assets);

    Board board;
    BoardInit(&board);

    SettingsState settings;
    SettingsDefaults(&settings);
    LoadSettings(&settings);

    int savedLevel = board.currentLevel;
    LoadProgress(&savedLevel);
    BoardLoadLevel(&board, savedLevel);
    SettingsApply(&settings);

    RenderTexture2D gameScene = LoadRenderTexture(SCREEN_W, SCREEN_H);
    SetTextureFilter(gameScene.texture, TEXTURE_FILTER_BILINEAR);
    int texelSizeLocation = GetShaderLocation(assets.shaderBlur, "texelSize");
    Vector2 texelSize = {1.0f / SCREEN_W, 1.0f / SCREEN_H};
    SetShaderValue(assets.shaderBlur, texelSizeLocation, &texelSize,
                   SHADER_UNIFORM_VEC2);

    HudLayout hudLayout = HudGetLayout();
    PauseLayout pauseLayout = PauseGetLayout(&assets);
    CompletionLayout completionLayout = CompletionGetLayout();
    MenuLayout menuLayout = MenuGetLayout(&assets);
    LevelSelectLayout levelSelectLayout = LevelSelectGetLayout(&assets);
    SettingsLayout settingsLayout = SettingsGetLayout(&assets);

    CheatsheetInit();
    CheatsheetLayout cheatsheetLayout = CheatsheetGetLayout(&assets, SCREEN_W,
                                                            SCREEN_H);
    HighScoresLayout highScoresLayout = HighScoresGetLayout(&assets, SCREEN_W,
                                                            SCREEN_H);
    ControlsLayout controlsLayout = ControlsGetLayout(&assets, SCREEN_W,
                                                      SCREEN_H);
    CreditsLayout creditsLayout = CreditsGetLayout(&assets, SCREEN_W,
                                                   SCREEN_H);
    CheatsheetState cheatsheet = {0};
    HighScoresState highScores = {0};

    bool showMenu = true;
    bool showLevelSelect = false;
    bool showSettings = false;
    bool showCheatsheet = false;
    bool showHighScores = false;
    bool showControls = false;
    bool showCredits = false;
    bool isPaused = false;

    bool hintShown = false;
    int hintPushCount = 0;
    int hintLevel = -1;
    bool scoreRecorded = false;

    float completionElapsed = 0.0f;
    int playedStarSounds = 0;

    Rectangle sceneSource = {
        0, 0, gameScene.texture.width, -gameScene.texture.height
    };
    Rectangle sceneDestination = {0, 0, SCREEN_W, SCREEN_H};

    while (!WindowShouldClose())
    {
        bool useMenuMusic = showMenu || showLevelSelect ||
            showSettings || showCheatsheet || showHighScores ||
            showControls || showCredits;
        AudioSetNonGameplayActive(useMenuMusic);

        AudioUpdate(GetFrameTime(), isPaused && !showSettings,
                    board.levelSolved != 0);

        Vector2 mouse = GetMousePosition();

        if (showSettings)
        {
            SettingsResult result = SettingsUpdate(&settingsLayout, &settings,
                                                   mouse);
            if (result == SETTINGS_ERASED)
            {
                LoadLevel(&board, 0);
                SaveProgress(0);

                isPaused = false;
                settings.origin = SETTINGS_FROM_MENU;
            }
            else if (result == SETTINGS_CLOSED)
            {
                showSettings = false;
                SaveSettings(&settings);
                if (settings.origin == SETTINGS_FROM_MENU)
                {
                    showMenu = true;
                }
            }

            BeginDrawing();
            ClearBackground(BLACK);
            SettingsDraw(&assets, &settingsLayout, &settings);
            EndDrawing();
            continue;
        }

        if (showHighScores)
        {
            HighScoresControl hovered =
                HighScoresHitTest(&highScoresLayout, &highScores, mouse);
            UiCursorAndSound(hovered != HIGHSCORE_CONTROL_NONE);

            bool shouldClose = IsKeyPressed(KEY_ESCAPE);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (hovered == HIGHSCORE_CONTROL_BACK)
                    shouldClose = true;
                else if (hovered == HIGHSCORE_CONTROL_PREV)
                    HighScoresStepPage(&highScores, -1);
                else if (hovered == HIGHSCORE_CONTROL_NEXT)
                    HighScoresStepPage(&highScores, 1);
            }

            float scoreWheel = GetMouseWheelMove();
            int scorePageDelta = 0;
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_DOWN) ||
                IsKeyPressed(KEY_PAGE_DOWN))
                scorePageDelta = 1;
            else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_UP) ||
                     IsKeyPressed(KEY_PAGE_UP))
                scorePageDelta = -1;
            else if (scoreWheel < 0)
                scorePageDelta = 1;
            else if (scoreWheel > 0)
                scorePageDelta = -1;
            if (scorePageDelta != 0)
                HighScoresStepPage(&highScores, scorePageDelta);

            if (shouldClose)
            {
                showHighScores = false;
                showLevelSelect = true;
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }

            BeginDrawing();
            ClearBackground(BLACK);
            HighScoresDraw(&assets, &highScoresLayout, &highScores, hovered);
            EndDrawing();
            continue;
        }

        if (showCredits)
        {
            CreditsControl hovered = CreditsHitTest(&creditsLayout, mouse);
            UiCursorAndSound(hovered != CREDITS_CONTROL_NONE);

            bool shouldClose = IsKeyPressed(KEY_ESCAPE) ||
                (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                 hovered == CREDITS_CONTROL_BACK);
            if (shouldClose)
            {
                showCredits = false;
                showMenu = true;
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }

            BeginDrawing();
            ClearBackground(BLACK);
            CreditsDraw(&assets, &creditsLayout, hovered);
            EndDrawing();
            continue;
        }

        if (showControls)
        {
            ControlsControl hovered = ControlsHitTest(&controlsLayout, mouse);
            UiCursorAndSound(hovered != CONTROLS_CONTROL_NONE);

            bool shouldClose = IsKeyPressed(KEY_ESCAPE) ||
                (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                 hovered == CONTROLS_CONTROL_BACK);
            if (shouldClose)
            {
                showControls = false;
                showLevelSelect = true;
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }

            BeginDrawing();
            ClearBackground(BLACK);
            ControlsDraw(&assets, &controlsLayout, hovered);
            EndDrawing();
            continue;
        }

        if (showCheatsheet)
        {
            CheatsheetControl hovered =
                CheatsheetHitTest(&cheatsheetLayout, &cheatsheet, mouse);
            UiCursorAndSound(hovered != CHEAT_CONTROL_NONE);

            bool shouldClose = IsKeyPressed(KEY_ESCAPE);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (hovered == CHEAT_CONTROL_BACK)
                    shouldClose = true;
                else if (hovered == CHEAT_CONTROL_PREV)
                    CheatsheetStepLevel(&cheatsheet, -1);
                else if (hovered == CHEAT_CONTROL_NEXT)
                    CheatsheetStepLevel(&cheatsheet, 1);
                else if (hovered == CHEAT_CONTROL_REVEAL)
                    CheatsheetToggleReveal(&cheatsheet);
                else if (hovered == CHEAT_CONTROL_PAGE_PREV)
                    CheatsheetStepPage(&cheatsheet, &cheatsheetLayout, -1);
                else if (hovered == CHEAT_CONTROL_PAGE_NEXT)
                    CheatsheetStepPage(&cheatsheet, &cheatsheetLayout, 1);
            }

            if (IsKeyPressed(KEY_LEFT))
                CheatsheetStepLevel(&cheatsheet, -1);
            if (IsKeyPressed(KEY_RIGHT))
                CheatsheetStepLevel(&cheatsheet, 1);
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_R))
                CheatsheetToggleReveal(&cheatsheet);

            float wheel = GetMouseWheelMove();
            int pageDelta = 0;
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_PAGE_DOWN))
                pageDelta = 1;
            else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_PAGE_UP))
                pageDelta = -1;
            else if (wheel < 0)
                pageDelta = 1;
            else if (wheel > 0)
                pageDelta = -1;
            if (pageDelta != 0)
                CheatsheetStepPage(&cheatsheet, &cheatsheetLayout, pageDelta);

            if (shouldClose)
            {
                showCheatsheet = false;
                showLevelSelect = true;
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }

            BeginDrawing();
            ClearBackground(BLACK);
            CheatsheetDraw(&assets, &cheatsheetLayout, &cheatsheet, hovered);
            EndDrawing();
            continue;
        }

        if (showMenu)
        {
            int hoveredMenuItem = MenuHitTest(&menuLayout, mouse);
            UiCursorAndSound(hoveredMenuItem >= 0);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (hoveredMenuItem == MENU_ITEM_CONTINUE)
                {
                    showMenu = false;
                }
                else if (hoveredMenuItem == MENU_ITEM_LEVEL_SELECT)
                {
                    showMenu = false;
                    showLevelSelect = true;
                }
                else if (hoveredMenuItem == MENU_ITEM_SETTINGS)
                {
                    showMenu = false;
                    showSettings = true;
                    settings.origin = SETTINGS_FROM_MENU;
                }
                else if (hoveredMenuItem == MENU_ITEM_CREDITS)
                {
                    showMenu = false;
                    showCredits = true;
                }
                else if (hoveredMenuItem == MENU_ITEM_QUIT)
                    break;
            }

            BeginDrawing();
            ClearBackground(BLACK);
            MenuDraw(&board, &assets, &menuLayout, hoveredMenuItem);
            EndDrawing();
            continue;
        }

        if (showLevelSelect)
        {
            LevelSelectHover hover =
                LevelSelectHitTest(&levelSelectLayout, mouse);
            UiCursorAndSound(LevelSelectHoveringControl(hover));

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (hover.back)
                {
                    showLevelSelect = false;
                    showMenu = true;
                }
                else if (hover.controls)
                {
                    showLevelSelect = false;
                    showControls = true;
                }
                else if (hover.shelf == LEVEL_SHELF_HIGH_SCORES)
                {
                    showLevelSelect = false;
                    showHighScores = true;
                    HighScoresOpen(&highScores, board.currentLevel);
                }
                else if (hover.shelf == LEVEL_SHELF_CHEATSHEETS)
                {
                    showLevelSelect = false;
                    showCheatsheet = true;
                    CheatsheetOpen(&cheatsheet, board.currentLevel);
                }
                else if (hover.card >= 0)
                {
                    LoadLevel(&board, hover.card);
                    showLevelSelect = false;
                }
                else if (hover.lockedCard >= 0)
                {
                    AudioPlay(SFX_UI_DENIED);
                }
            }
            if (IsKeyPressed(KEY_ESCAPE))
            {
                showLevelSelect = false;
                showMenu = true;
            }

            BeginDrawing();
            ClearBackground(BLACK);
            LevelSelectDraw(&board, &assets, &levelSelectLayout, hover);
            EndDrawing();
            continue;
        }

        bool gameplayActive = !isPaused && !board.levelSolved;
        bool hoveringHint = gameplayActive &&
            CheckCollisionPointRec(mouse, hudLayout.hintButton);
        bool hoveringUndo = gameplayActive && board.historyCount > 0 &&
            CheckCollisionPointRec(mouse, hudLayout.undoButton);
        bool hoveringRestart = gameplayActive &&
            CheckCollisionPointRec(mouse, hudLayout.restartButton);
        bool hoveringPause = gameplayActive &&
            CheckCollisionPointRec(mouse, hudLayout.pauseButton);
        bool hoveringResume =
            isPaused && CheckCollisionPointRec(mouse, pauseLayout.resumeButton);
        bool hoveringPauseRestart =
            isPaused && CheckCollisionPointRec(mouse,
                                               pauseLayout.restartButton);
        bool hoveringPauseSettings =
            isPaused && CheckCollisionPointRec(mouse,
                                               pauseLayout.settingsButton);
        bool hoveringPauseHome =
            isPaused && CheckCollisionPointRec(mouse, pauseLayout.homeButton);
        bool hoveringCompletionNext = board.levelSolved &&
            CheckCollisionPointRec(mouse, completionLayout.nextButton);
        bool hoveringCompletionReplay = board.levelSolved &&
            CheckCollisionPointRec(mouse, completionLayout.replayButton);
        bool hoveringCompletionHome = board.levelSolved &&
            CheckCollisionPointRec(mouse, completionLayout.homeButton);

        UiCursorAndSound(hoveringHint || hoveringUndo ||
                         hoveringRestart || hoveringPause ||
                         hoveringResume || hoveringPauseRestart ||
                         hoveringPauseSettings ||
                         hoveringCompletionNext || hoveringCompletionReplay ||
                         hoveringPauseHome || hoveringCompletionHome);

        bool leftMouseClicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (isPaused)
        {
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P) ||
                (leftMouseClicked && hoveringResume))
            {
                isPaused = false;
            }
            else if (leftMouseClicked && hoveringPauseRestart)
            {
                AudioPlay(SFX_RESTART);
                BoardLoadLevel(&board, board.currentLevel);
                isPaused = false;
                AudioRestartMusic();
            }
            else if (leftMouseClicked && hoveringPauseSettings)
            {
                showSettings = true;
                settings.origin = SETTINGS_FROM_PAUSE;
            }
            else if (leftMouseClicked && hoveringPauseHome)
            {
                isPaused = false;
                showMenu = true;
            }
        }
        else
        {
#if DEV_LEVEL_SKIP
            if (IsKeyPressed(KEY_RIGHT_BRACKET))
                LoadLevel(&board, board.currentLevel + 1);
            else if (IsKeyPressed(KEY_LEFT_BRACKET))
                LoadLevel(&board, board.currentLevel - 1);
#endif

            if (IsKeyPressed(KEY_M))
                AudioToggleMute();

            if ((leftMouseClicked && hoveringHint) || IsKeyPressed(KEY_H))
            {
                hintShown = true;
                hintPushCount = board.pushCount;
                hintLevel = board.currentLevel;
            }

            if ((leftMouseClicked && hoveringUndo) || IsKeyPressed(KEY_U))
                BoardUndo(&board);

            bool pauseRequested = IsKeyPressed(KEY_P) ||
                (leftMouseClicked && hoveringPause);

            if (pauseRequested && !board.levelSolved)
                isPaused = true;
            else
            {
                BoardUpdate(&board, GetFrameTime());

                if (board.levelSolved && !scoreRecorded)
                {
                    ScoreFileRun(&board);
                    SaveProgress(board.currentLevel);
                    scoreRecorded = true;
                    completionElapsed = 0.0f;
                    playedStarSounds = 0;
                }
                else if (!board.levelSolved)
                {
                    scoreRecorded = false;
                    completionElapsed = 0.0f;
                    playedStarSounds = 0;
                }

                if (board.levelSolved)
                {
                    completionElapsed += GetFrameTime();
                    CompletionPlayStarTicks(&board, completionElapsed,
                                            &playedStarSounds);
                }

                if (board.levelSolved && leftMouseClicked &&
                    hoveringCompletionHome)
                {
                    LoadLevel(&board, board.currentLevel + 1);
                    showMenu = true;
                }
                else if (board.levelSolved &&
                    (IsKeyPressed(KEY_ENTER) ||
                     (leftMouseClicked && hoveringCompletionNext)))
                {
                    LoadLevel(&board, board.currentLevel + 1);
                }
                else if ((board.levelSolved &&
                          (IsKeyPressed(KEY_R) ||
                           (leftMouseClicked && hoveringCompletionReplay))) ||
                         (!board.levelSolved &&
                          (IsKeyPressed(KEY_R) ||
                           (leftMouseClicked && hoveringRestart))))
                {
                    AudioPlay(SFX_RESTART);
                    BoardLoadLevel(&board, board.currentLevel);
                    AudioRestartMusic();
                }
            }
        }

        if (hintShown && (board.pushCount != hintPushCount ||
                          board.currentLevel != hintLevel))
            hintShown = false;

        Hint hint = HintAsk(&board);
        hint.active = hintShown;
        if (!hintShown)
            hint.status = HINT_IDLE;

        BeginTextureMode(gameScene);
        ClearBackground(COL_BG);
        HudDraw(&board, &assets, &hudLayout, &hint,
                (HudHover){hoveringHint, hoveringUndo, hoveringRestart,
                           hoveringPause});
        if (board.levelSolved)
            CompletionDraw(&board, &assets, completionLayout,
                           hoveringCompletionNext, hoveringCompletionReplay,
                           hoveringCompletionHome, completionElapsed);
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        if (isPaused)
        {
            BeginShaderMode(assets.shaderBlur);
            DrawTexturePro(gameScene.texture, sceneSource, sceneDestination,
                           (Vector2){0, 0}, 0, WHITE);
            EndShaderMode();
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.52f));
            PauseDraw(&board, &assets, &pauseLayout,
                      hoveringResume, hoveringPauseRestart,
                      hoveringPauseSettings, hoveringPauseHome);
        }
        else
        {
            DrawTexturePro(gameScene.texture, sceneSource, sceneDestination,
                           (Vector2){0, 0}, 0, WHITE);
        }

        EndDrawing();
    }

    SaveProgress(board.currentLevel);
    SaveSettings(&settings);

    UnloadRenderTexture(gameScene);
    HintUnload();
    AssetsUnload(&assets);
    AudioShutdown();
    CloseWindow();
    return 0;
}
