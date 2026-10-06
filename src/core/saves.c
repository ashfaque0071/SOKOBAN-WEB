#include "saves.h"

#include <stdio.h>

#if defined(PLATFORM_WEB)
#include <emscripten.h>
#endif

#include "raymath.h"
#include "board.h"
#include "score.h"

#if defined(PLATFORM_WEB)
/* The browser build mounts IndexedDB at /data, so the save paths stay
   absolute there rather than depending on the working directory. */
#define PROGRESS_PATH "/data/progress.txt"
#define SETTINGS_PATH "/data/settings.txt"
#else
#define PROGRESS_PATH "data/progress.txt"
#define SETTINGS_PATH "data/settings.txt"
#endif

#define PROGRESS_VERSION 4
#define SETTINGS_VERSION 2

/* Versions 3 and 4 reordered and expanded the campaign. Translate older
   index-based saves so completed levels and records stay attached to the
   same boards after each reorder. */
static int ProgressIndexForVersion(int version, int index)
{
    static const unsigned char version2ToVersion3[48] = {
        0, 1, 2, 3, 4, 6, 11, 15,
        17, 19, 26, 27, 34, 37, 44, 47,
        10, 16, 24, 25, 32, 36, 42, 46,
        9, 13, 23, 20, 31, 35, 41, 45,
        7, 12, 22, 18, 30, 33, 39, 43,
        5, 8, 21, 14, 28, 29, 38, 40
    };
    static const unsigned char version3ToVersion4[49] = {
        0, 1, 2, 3, 4, 5, 6, 7,
        8, 9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 21, 24, 25, 28,
        29, 35, 37, 41, 42, 44, 47, 48,
        50, 53, 54, 55, 57, 60, 63, 65,
        67, 68, 69, 74, 76, 77, 78, 83,
        99
    };

    if (version < 3)
        index = index >= 0 && index < 48 ? version2ToVersion3[index] : -1;
    if (version < 4)
        index = index >= 0 && index < 49 ? version3ToVersion4[index] : -1;
    return index;
}

#if defined(PLATFORM_WEB)
/* An IDBFS write only reaches memory; syncfs is what commits it to IndexedDB
   so the save survives a page reload. */
static void SavesCommit(void)
{
    /* emscripten_run_script rather than EM_ASM, which needs a GNU dialect and
       would split the web build off the project's -std=c99. This runs once per
       save, so the eval costs nothing that matters. */
    emscripten_run_script(
        "FS.syncfs(false, function (err) {"
        "  if (err) console.error('sokoban: save sync failed', err);"
        "});");
}
#else
#define SavesCommit() ((void)0)
#endif

void SaveProgress(int currentLevel)
{
    FILE *file = fopen(PROGRESS_PATH, "w");
    if (!file)
        return;

    fprintf(file, "%i\n", PROGRESS_VERSION);
    fprintf(file, "%i %i\n", currentLevel, LEVEL_COUNT);
    for (int i = 0; i < LEVEL_COUNT; i++)
        fprintf(file, "%i %i %i %i\n", LevelBestScore(i), LevelBestStars(i),
                LevelBestPushes(i), LevelBestMoves(i));

    fclose(file);
    SavesCommit();
}

void LoadProgress(int *currentLevel)
{
    FILE *file = fopen(PROGRESS_PATH, "r");
    if (!file)
        return;

    int version = 0;
    if (fscanf(file, "%i", &version) != 1 ||
        version < 1 || version > PROGRESS_VERSION)
    {
        fclose(file);
        return;
    }

    int level = 0;
    int count = 0;
    if (fscanf(file, "%i %i", &level, &count) == 2)
    {
        int mappedLevel = ProgressIndexForVersion(version, level);
        if (mappedLevel >= 0 && mappedLevel < LEVEL_COUNT)
            *currentLevel = mappedLevel;
        int maximumCount = version < 3 ? 48 : (version < 4 ? 49 : LEVEL_COUNT);
        if (count < 0)
            count = 0;
        if (count > maximumCount)
            count = maximumCount;
        for (int i = 0; i < count; i++)
        {
            int score = 0;
            int stars = 0;
            if (fscanf(file, "%i %i", &score, &stars) != 2)
                break;

            int pushes = 0;
            int moves = 0;
            if (version >= 2 && fscanf(file, "%i %i", &pushes, &moves) != 2)
            {
                ScoreSetRecord(i, score, stars, 0, 0);
                break;
            }
            int mappedIndex = ProgressIndexForVersion(version, i);
            if (mappedIndex >= 0 && mappedIndex < LEVEL_COUNT)
                ScoreSetRecord(mappedIndex, score, stars, pushes, moves);
        }
    }

    fclose(file);
}

void SaveSettings(const SettingsState *state)
{
    FILE *file = fopen(SETTINGS_PATH, "w");
    if (!file)
        return;

    fprintf(file, "%i\n", SETTINGS_VERSION);
    fprintf(file, "%i %.3f %.3f\n", state->masterSoundOn ? 1 : 0,
            state->musicVolume, state->effectsVolume);
    fprintf(file, "%i %i\n", state->holdToRepeat, state->speedIndex);

    fclose(file);
    SavesCommit();
}

void LoadSettings(SettingsState *state)
{
    FILE *file = fopen(SETTINGS_PATH, "r");
    if (!file)
        return;

    int version = 0;
    if (fscanf(file, "%i", &version) != 1 ||
        version < 1 || version > SETTINGS_VERSION)
    {
        fclose(file);
        return;
    }

    int sound = 0;
    float music = 0;
    float effects = 0;
    if (fscanf(file, "%i %f %f", &sound, &music, &effects) == 3)
    {
        state->masterSoundOn = (sound != 0);
        /* Version 2 changes the shipped default to silent music. Apply that
           once to existing settings so returning browser players get the new
           behavior too; they can still raise the slider afterward. */
        state->musicVolume = version < 2 ? 0.0f : Clamp(music, 0.0f, 1.0f);
        state->effectsVolume = Clamp(effects, 0.0f, 1.0f);
    }

    int repeat = 0;
    int speed = 0;
    if (fscanf(file, "%i %i", &repeat, &speed) == 2)
    {
        state->holdToRepeat = (repeat != 0);
        if (speed >= 0 && speed < SETTINGS_SPEED_COUNT)
            state->speedIndex = speed;
    }

    fclose(file);
}
