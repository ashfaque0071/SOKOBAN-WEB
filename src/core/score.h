#ifndef SCORE_H
#define SCORE_H

#include "board.h"

int GetCompletionStarRating(const Board *board);
int GetPushScore(const Board *board);
int GetMoveBonus(const Board *board);
int GetLevelScore(const Board *board);

void FormatScore(char *out, int value);

bool ScoreFileRun(const Board *board);

bool ScoreIsNewBest(void);

void ScoreSetRecord(int index, int score, int stars, int pushes, int moves);
void ScoreResetAll(void);

bool LevelUnlocked(int index);

bool LevelCleared(int index);
int LevelMinimumPushes(int index);
int LevelBestScore(int index);
int LevelBestStars(int index);
int LevelBestPushes(int index);
int LevelBestMoves(int index);

#endif
