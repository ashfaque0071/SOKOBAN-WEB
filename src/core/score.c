#include "score.h"

#include "raylib.h"

static const int MINIMUM_PUSHES[LEVEL_COUNT] = {
    7, 5, 5, 6, 10, 9, 13, 15,
    11, 16, 13, 16, 34, 25, 48, 43
};

static int levelBestScore[LEVEL_COUNT];
static int levelBestStars[LEVEL_COUNT];

static int levelBestPushes[LEVEL_COUNT];
static int levelBestMoves[LEVEL_COUNT];
static bool completionIsNewBest = false;

int GetCompletionStarRating(const Board *board)
{
    int minimum = MINIMUM_PUSHES[board->currentLevel];

    if (!board->levelSolved)
        return 0;
    if (board->pushCount <= minimum)
        return 3;

    if (board->pushCount * 4 <= minimum * 5)
        return 2;
    return 1;
}

int GetPushScore(const Board *board)
{
    if (board->pushCount <= 0)
        return 0;
    return 10000 * MINIMUM_PUSHES[board->currentLevel] / board->pushCount;
}

int GetMoveBonus(const Board *board)
{
    int bonus = 1000 - board->moveCount * 5;
    return bonus > 0 ? bonus : 0;
}

int GetLevelScore(const Board *board)
{
    if (!board->levelSolved)
        return 0;
    return GetPushScore(board) + GetMoveBonus(board);
}

void FormatScore(char *out, int value)
{
    char digits[16];
    TextCopy(digits, TextFormat("%i", value));

    int length = TextLength(digits);
    int written = 0;
    for (int i = 0; i < length; i++)
    {
        if (i > 0 && (length - i) % 3 == 0)
            out[written++] = ',';
        out[written++] = digits[i];
    }
    out[written] = '\0';
}

bool ScoreFileRun(const Board *board)
{
    int level = board->currentLevel;
    int score = GetLevelScore(board);
    int stars = GetCompletionStarRating(board);

    completionIsNewBest = score > levelBestScore[level];
    if (completionIsNewBest)
    {
        levelBestScore[level] = score;

        levelBestPushes[level] = board->pushCount;
        levelBestMoves[level] = board->moveCount;
    }

    if (stars > levelBestStars[level])
        levelBestStars[level] = stars;

    return completionIsNewBest;
}

bool ScoreIsNewBest(void)
{
    return completionIsNewBest;
}

static bool IndexInRange(int index)
{
    return index >= 0 && index < LEVEL_COUNT;
}

void ScoreSetRecord(int index, int score, int stars, int pushes, int moves)
{
    if (!IndexInRange(index))
        return;

    levelBestScore[index] = score > 0 ? score : 0;
    levelBestStars[index] = (stars >= 0 && stars <= 3) ? stars : 0;
    levelBestPushes[index] = pushes > 0 ? pushes : 0;
    levelBestMoves[index] = moves > 0 ? moves : 0;
}

void ScoreResetAll(void)
{
    for (int i = 0; i < LEVEL_COUNT; i++)
    {
        levelBestScore[i] = 0;
        levelBestStars[i] = 0;
        levelBestPushes[i] = 0;
        levelBestMoves[i] = 0;
    }
    completionIsNewBest = false;
}

bool LevelUnlocked(int index)
{
    return index == 0 || levelBestScore[index - 1] > 0;
}

bool LevelCleared(int index)
{
    return IndexInRange(index) && levelBestScore[index] > 0;
}

int LevelMinimumPushes(int index)
{
    return IndexInRange(index) ? MINIMUM_PUSHES[index] : 0;
}

int LevelBestScore(int index)
{
    return IndexInRange(index) ? levelBestScore[index] : 0;
}

int LevelBestStars(int index)
{
    return IndexInRange(index) ? levelBestStars[index] : 0;
}

int LevelBestPushes(int index)
{
    return IndexInRange(index) ? levelBestPushes[index] : 0;
}

int LevelBestMoves(int index)
{
    return IndexInRange(index) ? levelBestMoves[index] : 0;
}
