#include "raylib.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

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
#include "web_input.h"

/* Allow [ and ] to move to the previous or next level in every build. */
#define DEV_LEVEL_SKIP 1

/* The browser drives the game one frame at a time through a callback, so the
   frame body cannot keep its working state in main's stack frame. Everything
   the loop touches lives here instead, and UpdateDrawFrame returns where the
   original loop said "continue". */
static Assets assets;
static Board board;
static SettingsState settings;

static RenderTexture2D gameScene;
static Rectangle sceneSource;
static Rectangle sceneDestination;

static HudLayout hudLayout;
static PauseLayout pauseLayout;
static CompletionLayout completionLayout;
static MenuLayout menuLayout;
static LevelSelectLayout levelSelectLayout;
static SettingsLayout settingsLayout;
static CheatsheetLayout cheatsheetLayout;
static HighScoresLayout highScoresLayout;
static ControlsLayout controlsLayout;
static CreditsLayout creditsLayout;

static CheatsheetState cheatsheet;
static HighScoresState highScores;

static bool showMenu = true;
static bool showLevelSelect = false;
static int levelSelectPage = 0;
static bool showSettings = false;
static bool showCheatsheet = false;
static bool showHighScores = false;
static bool showControls = false;
static bool showCredits = false;
static bool isPaused = false;

static bool hintShown = false;
static int hintPushCount = 0;
static int hintLevel = -1;
static bool scoreRecorded = false;

static float completionElapsed = 0.0f;
static int playedStarSounds = 0;

#if defined(PLATFORM_WEB)
static bool contentLoaded = false;
static int queuedMenuItem = -1;
static bool preparingContent = false;
#endif

#if !defined(PLATFORM_WEB)
/* Only the native loop has a flag to clear; the web build stops by cancelling
   its frame callback. */
static bool appRunning = true;
#endif

static void LoadLevel(Board *target, int levelIndex)
{
    AudioStop(SFX_LEVEL_COMPLETE);
    BoardLoadLevel(target, (levelIndex + LEVEL_COUNT) % LEVEL_COUNT);
    AudioRestartMusic();
    /* Native saves again from AppShutdown, but a closing browser tab gives no
       such chance, so the level is persisted the moment it changes. */
    SaveProgress(target->currentLevel);
}

static void AppLoadDeferred(void)
{
#if defined(PLATFORM_WEB)
    AssetsLoadDeferred(&assets);
    AudioLoadDeferred();
#endif
    gameScene = LoadRenderTexture(SCREEN_W, SCREEN_H);
    SetTextureFilter(gameScene.texture, TEXTURE_FILTER_BILINEAR);
    int texelSizeLocation = GetShaderLocation(assets.shaderBlur, "texelSize");
    Vector2 texelSize = {1.0f / SCREEN_W, 1.0f / SCREEN_H};
    SetShaderValue(assets.shaderBlur, texelSizeLocation, &texelSize,
                   SHADER_UNIFORM_VEC2);

    hudLayout = HudGetLayout();
    pauseLayout = PauseGetLayout(&assets);
    completionLayout = CompletionGetLayout();
    levelSelectLayout = LevelSelectGetLayout(&assets);
    settingsLayout = SettingsGetLayout(&assets);
    CheatsheetInit();
    cheatsheetLayout = CheatsheetGetLayout(&assets, SCREEN_W, SCREEN_H);
    highScoresLayout = HighScoresGetLayout(&assets, SCREEN_W, SCREEN_H);
    controlsLayout = ControlsGetLayout(&assets, SCREEN_W, SCREEN_H);
    creditsLayout = CreditsGetLayout(&assets, SCREEN_W, SCREEN_H);
    sceneSource = (Rectangle){
        0, 0, gameScene.texture.width, -gameScene.texture.height
    };
    sceneDestination = (Rectangle){0, 0, SCREEN_W, SCREEN_H};
}

static void AppInit(void)
{
    InitWindow(SCREEN_W, SCREEN_H, "Sokoban");
    AudioInit();
    SetTargetFPS(60);
    AssetsLoad(&assets);

    BoardInit(&board);

    SettingsDefaults(&settings);
    LoadSettings(&settings);

    int savedLevel = board.currentLevel;
    LoadProgress(&savedLevel);
    BoardLoadLevel(&board, savedLevel);
    SettingsApply(&settings);

    menuLayout = MenuGetLayout(&assets);
#if !defined(PLATFORM_WEB)
    AppLoadDeferred();
#endif
}

#if !defined(PLATFORM_WEB)
static void AppShutdown(void)
{
    SaveProgress(board.currentLevel);
    SaveSettings(&settings);

    UnloadRenderTexture(gameScene);
    HintUnload();
    AssetsUnload(&assets);
    AudioShutdown();
    CloseWindow();
}
#endif

/* Quit means "end the session". Natively that drops out of the loop and runs
   AppShutdown; a browser tab cannot close itself, so the web build saves, stops
   its frame loop and hands the page back to the shell, which offers to start
   again. */
static void AppQuit(void)
{
#if defined(PLATFORM_WEB)
    SaveProgress(board.currentLevel);
    SaveSettings(&settings);
    emscripten_cancel_main_loop();
    emscripten_run_script("if (window.sokobanQuit) window.sokobanQuit();");
#else
    appRunning = false;
#endif
}

static void OpenMenuItem(int item)
{
    if (item == MENU_ITEM_CONTINUE)
        showMenu = false;
    else if (item == MENU_ITEM_LEVEL_SELECT)
    {
        showMenu = false;
        showLevelSelect = true;
        levelSelectPage = board.currentLevel / LEVEL_SLOTS;
    }
    else if (item == MENU_ITEM_SETTINGS)
    {
        showMenu = false;
        showSettings = true;
        settings.origin = SETTINGS_FROM_MENU;
    }
    else if (item == MENU_ITEM_CREDITS)
    {
        showMenu = false;
        showCredits = true;
    }
}

static void UpdateDrawFrame(void)
{
        InputBeginFrame();
        int inputScreen = INPUT_PLAY;
        if (showSettings) inputScreen = INPUT_SETTINGS;
        else if (showHighScores) inputScreen = INPUT_SCORES;
        else if (showCredits) inputScreen = INPUT_CREDITS;
        else if (showControls) inputScreen = INPUT_CONTROLS;
        else if (showCheatsheet) inputScreen = INPUT_CHEATS;
        else if (showMenu) inputScreen = INPUT_MENU;
        else if (showLevelSelect) inputScreen = INPUT_LEVELS;
        else if (isPaused) inputScreen = INPUT_PAUSED;
        else if (board.levelSolved) inputScreen = INPUT_SOLVED;
        InputSetScreen(inputScreen);

        bool useMenuMusic = showMenu || showLevelSelect ||
            showSettings || showCheatsheet || showHighScores ||
            showControls || showCredits;
        AudioSetNonGameplayActive(useMenuMusic);

        AudioUpdate(GetFrameTime(), isPaused && !showSettings,
                    board.levelSolved != 0);

        Vector2 mouse = InputPointerPosition();

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
            return;
        }

        if (showHighScores)
        {
            HighScoresControl hovered =
                HighScoresHitTest(&highScoresLayout, &highScores, mouse);
            UiCursorAndSound(hovered != HIGHSCORE_CONTROL_NONE);

            bool shouldClose = InputKeyPressed(KEY_ESCAPE);
            if (InputPointerPressed())
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
            if (InputKeyPressed(KEY_RIGHT) || InputKeyPressed(KEY_DOWN) ||
                InputKeyPressed(KEY_PAGE_DOWN))
                scorePageDelta = 1;
            else if (InputKeyPressed(KEY_LEFT) || InputKeyPressed(KEY_UP) ||
                     InputKeyPressed(KEY_PAGE_UP))
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
            return;
        }

        if (showCredits)
        {
            CreditsControl hovered = CreditsHitTest(&creditsLayout, mouse);
            UiCursorAndSound(hovered != CREDITS_CONTROL_NONE);

            bool shouldClose = InputKeyPressed(KEY_ESCAPE) ||
                (InputPointerPressed() &&
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
            return;
        }

        if (showControls)
        {
            ControlsControl hovered = ControlsHitTest(&controlsLayout, mouse);
            UiCursorAndSound(hovered != CONTROLS_CONTROL_NONE);

            bool shouldClose = InputKeyPressed(KEY_ESCAPE) ||
                (InputPointerPressed() &&
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
            return;
        }

        if (showCheatsheet)
        {
            CheatsheetControl hovered =
                CheatsheetHitTest(&cheatsheetLayout, &cheatsheet, mouse);
            UiCursorAndSound(hovered != CHEAT_CONTROL_NONE);

            bool shouldClose = InputKeyPressed(KEY_ESCAPE);
            if (InputPointerPressed())
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

            if (InputKeyPressed(KEY_LEFT))
                CheatsheetStepLevel(&cheatsheet, -1);
            if (InputKeyPressed(KEY_RIGHT))
                CheatsheetStepLevel(&cheatsheet, 1);
            if (InputKeyPressed(KEY_ENTER) || InputKeyPressed(KEY_R))
                CheatsheetToggleReveal(&cheatsheet);

            float wheel = GetMouseWheelMove();
            int pageDelta = 0;
            if (InputKeyPressed(KEY_DOWN) || InputKeyPressed(KEY_PAGE_DOWN))
                pageDelta = 1;
            else if (InputKeyPressed(KEY_UP) || InputKeyPressed(KEY_PAGE_UP))
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
            return;
        }

        if (showMenu)
        {
#if defined(PLATFORM_WEB)
            if (queuedMenuItem >= 0 &&
                emscripten_run_script_int("window.sokobanContentReady ? 1 : 0"))
            {
                if (!preparingContent)
                {
                    preparingContent = true;
                    emscripten_run_script("window.sokobanPreparingContent()");
                }
                else
                {
                    AppLoadDeferred();
                    contentLoaded = true;
                    preparingContent = false;
                    emscripten_run_script("window.sokobanContentPrepared()");
                    OpenMenuItem(queuedMenuItem);
                    queuedMenuItem = -1;
                }
            }
#endif
            int hoveredMenuItem = MenuHitTest(&menuLayout, mouse);
            UiCursorAndSound(hoveredMenuItem >= 0);

            int selectedMenuItem = InputPointerPressed() ? hoveredMenuItem : -1;
            if (selectedMenuItem >= 0)
            {
                if (selectedMenuItem == MENU_ITEM_QUIT)
                {
                    AppQuit();
                    return;
                }
#if defined(PLATFORM_WEB)
                else if (!contentLoaded)
                {
                    queuedMenuItem = selectedMenuItem;
                    emscripten_run_script("window.sokobanRequireContent()");
                }
#endif
                else
                    OpenMenuItem(selectedMenuItem);
            }

            BeginDrawing();
            ClearBackground(BLACK);
            MenuDraw(&board, &assets, &menuLayout, hoveredMenuItem);
            EndDrawing();
            return;
        }

        if (showLevelSelect)
        {
            LevelSelectHover hover =
                LevelSelectHitTest(&levelSelectLayout, levelSelectPage, mouse);
            UiCursorAndSound(LevelSelectHoveringControl(hover));

            if (InputPointerPressed())
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
                    if (LevelUnlocked(hover.card))
                    {
                        LoadLevel(&board, hover.card);
                        showLevelSelect = false;
                    }
                    else
                        AudioPlay(SFX_UI_DENIED);
                }
                else if (hover.lockedCard >= 0)
                {
                    AudioPlay(SFX_UI_DENIED);
                }
                else if (hover.prevPage)
                    levelSelectPage--;
                else if (hover.nextPage)
                    levelSelectPage++;
            }
            float levelWheel = GetMouseWheelMove();
            int levelPageDelta = 0;
            if (InputKeyPressed(KEY_RIGHT) || InputKeyPressed(KEY_DOWN) ||
                InputKeyPressed(KEY_PAGE_DOWN) || levelWheel < 0)
                levelPageDelta = 1;
            else if (InputKeyPressed(KEY_LEFT) || InputKeyPressed(KEY_UP) ||
                     InputKeyPressed(KEY_PAGE_UP) || levelWheel > 0)
                levelPageDelta = -1;
            if (levelPageDelta != 0)
            {
                int target = levelSelectPage + levelPageDelta;
                if (target >= 0 && target < LevelSelectPageCount())
                    levelSelectPage = target;
            }
            if (InputKeyPressed(KEY_ESCAPE))
            {
                showLevelSelect = false;
                showMenu = true;
            }

            BeginDrawing();
            ClearBackground(BLACK);
            LevelSelectDraw(&board, &assets, &levelSelectLayout,
                            levelSelectPage, hover);
            EndDrawing();
            return;
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

        bool leftMouseClicked = InputPointerPressed();

        if (isPaused)
        {
            if (InputKeyPressed(KEY_ESCAPE) || InputKeyPressed(KEY_P) ||
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
            if (InputKeyPressed(KEY_RIGHT_BRACKET))
                LoadLevel(&board, board.currentLevel + 1);
            else if (InputKeyPressed(KEY_LEFT_BRACKET))
                LoadLevel(&board, board.currentLevel - 1);
#endif

            if (InputKeyPressed(KEY_M))
                AudioToggleMute();

            if ((leftMouseClicked && hoveringHint) || InputKeyPressed(KEY_H))
            {
                hintShown = true;
                hintPushCount = board.pushCount;
                hintLevel = board.currentLevel;
            }

            if ((leftMouseClicked && hoveringUndo) || InputKeyPressed(KEY_U))
                BoardUndo(&board);

            bool pauseRequested = InputKeyPressed(KEY_P) ||
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

                if (board.levelSolved &&
                    (leftMouseClicked && hoveringCompletionHome))
                {
                    LoadLevel(&board, board.currentLevel + 1);
                    showMenu = true;
                }
                else if (board.levelSolved &&
                    (InputKeyPressed(KEY_ENTER) ||
                     (leftMouseClicked && hoveringCompletionNext)))
                {
                    LoadLevel(&board, board.currentLevel + 1);
                }
                else if ((board.levelSolved &&
                          (InputKeyPressed(KEY_R) ||
                           (leftMouseClicked && hoveringCompletionReplay))) ||
                         (!board.levelSolved &&
                          (InputKeyPressed(KEY_R) ||
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

int main(void)
{
    AppInit();

#if defined(PLATFORM_WEB)
    /* emscripten_set_main_loop hands control back to the browser between
       frames and never returns, so AppShutdown is unreachable here. Progress
       and settings are flushed as they change instead. */
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (appRunning && !WindowShouldClose())
        UpdateDrawFrame();

    AppShutdown();
#endif

    return 0;
}
