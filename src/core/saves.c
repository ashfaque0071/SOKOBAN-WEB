#include "saves.h"

#include <stdio.h>

#include "raymath.h"
#include "board.h"
#include "score.h"

#define PROGRESS_PATH "data/progress.txt"
#define SETTINGS_PATH "data/settings.txt"

#define PROGRESS_VERSION 2
#define SETTINGS_VERSION 1

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
        if (level >= 0 && level < LEVEL_COUNT)
            *currentLevel = level;
        if (count > LEVEL_COUNT)
            count = LEVEL_COUNT;
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
            ScoreSetRecord(i, score, stars, pushes, moves);
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
}

void LoadSettings(SettingsState *state)
{
    FILE *file = fopen(SETTINGS_PATH, "r");
    if (!file)
        return;

    int version = 0;
    if (fscanf(file, "%i", &version) != 1 || version != SETTINGS_VERSION)
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
        state->musicVolume = Clamp(music, 0.0f, 1.0f);
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
