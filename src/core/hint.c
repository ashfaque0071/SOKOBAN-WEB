#include "hint.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "cheatsheet.h"
#include "colors.h"
#include "ui.h"

#define HINT_MAX_CELLS 64
#define HINT_EMPTY 0xFFFFu

#define HINT_MAX_ROUTE 128

static const int HINT_DIR[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

typedef struct HintEntry {
    uint64_t mask;
    uint16_t player;
    uint16_t dist;
} HintEntry;

typedef struct HintLevel {
    bool attempted;
    bool usable;

    int cellCount;
    int cellRow[HINT_MAX_CELLS];
    int cellCol[HINT_MAX_CELLS];
    int cellAt[ROWS][COLS];
    int neighbour[HINT_MAX_CELLS][4];
    uint64_t goalMask;
    uint64_t startMask;
    int startPlayer;

    HintEntry *slot;
    size_t capacity;
    size_t count;

    uint64_t routeMask[HINT_MAX_ROUTE];
    uint16_t routePlayer[HINT_MAX_ROUTE];
    int routeCount;
} HintLevel;

static HintLevel hintLevels[LEVEL_COUNT];

static size_t HintHash(uint64_t mask, uint16_t player)
{
    uint64_t h = mask + 0x9E3779B97F4A7C15ULL;
    h ^= (uint64_t)player * 0xC2B2AE3D27D4EB4FULL;
    h ^= h >> 30; h *= 0xBF58476D1CE4E5B9ULL;
    h ^= h >> 27; h *= 0x94D049BB133111EBULL;
    h ^= h >> 31;
    return (size_t)h;
}

static HintEntry *HintFind(HintLevel *lv, uint64_t mask, uint16_t player)
{
    if (lv->capacity == 0)
        return NULL;

    size_t wrap = lv->capacity - 1;
    size_t i = HintHash(mask, player) & wrap;
    while (lv->slot[i].player != HINT_EMPTY)
    {
        if (lv->slot[i].mask == mask && lv->slot[i].player == player)
            return &lv->slot[i];
        i = (i + 1) & wrap;
    }
    return NULL;
}

static void HintPlace(HintEntry *slot, size_t capacity, uint64_t mask,
                      uint16_t player, uint16_t dist)
{
    size_t wrap = capacity - 1;
    size_t i = HintHash(mask, player) & wrap;
    while (slot[i].player != HINT_EMPTY)
        i = (i + 1) & wrap;
    slot[i].mask = mask;
    slot[i].player = player;
    slot[i].dist = dist;
}

static bool HintGrow(HintLevel *lv)
{
    size_t capacity = lv->capacity ? lv->capacity * 2 : 4096;
    HintEntry *slot = malloc(capacity * sizeof(HintEntry));
    if (!slot)
        return false;

    for (size_t i = 0; i < capacity; i++)
        slot[i].player = HINT_EMPTY;
    for (size_t i = 0; i < lv->capacity; i++)
        if (lv->slot[i].player != HINT_EMPTY)
            HintPlace(slot, capacity, lv->slot[i].mask,
                      lv->slot[i].player, lv->slot[i].dist);

    free(lv->slot);
    lv->slot = slot;
    lv->capacity = capacity;
    return true;
}

static bool HintInsert(HintLevel *lv, uint64_t mask, uint16_t player,
                       uint16_t dist)
{
    if ((lv->count + 1) * 10 >= lv->capacity * 6)
        if (!HintGrow(lv))
            return false;

    size_t wrap = lv->capacity - 1;
    size_t i = HintHash(mask, player) & wrap;
    while (lv->slot[i].player != HINT_EMPTY)
    {

        if (lv->slot[i].mask == mask && lv->slot[i].player == player)
            return false;
        i = (i + 1) & wrap;
    }

    lv->slot[i].mask = mask;
    lv->slot[i].player = player;
    lv->slot[i].dist = dist;
    lv->count++;
    return true;
}

static uint64_t HintReach(const HintLevel *lv, uint64_t crates, int from)
{
    uint64_t seen = 1ULL << from;
    int stack[HINT_MAX_CELLS];
    int top = 0;

    stack[top++] = from;
    while (top > 0)
    {
        int cell = stack[--top];
        for (int d = 0; d < 4; d++)
        {
            int next = lv->neighbour[cell][d];
            if (next < 0)
                continue;
            uint64_t bit = 1ULL << next;
            if (((seen | crates) & bit) != 0)
                continue;
            seen |= bit;
            stack[top++] = next;
        }
    }
    return seen;
}

static int HintNormalise(const HintLevel *lv, uint64_t crates, int from)
{
    return __builtin_ctzll(HintReach(lv, crates, from));
}

static bool HintGeometry(HintLevel *lv, int level)
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            lv->cellAt[r][c] = -1;

    lv->cellCount = 0;
    for (int r = 0; r < ROWS; r++)
    {
        const char *line = BoardLevelRow(level, r);
        if (!line)
            return false;
        for (int c = 0; c < COLS; c++)
        {
            char ch = line[c];
            if (ch == '#' || ch == '_')
                continue;
            if (lv->cellCount >= HINT_MAX_CELLS)
                return false;
            lv->cellAt[r][c] = lv->cellCount;
            lv->cellRow[lv->cellCount] = r;
            lv->cellCol[lv->cellCount] = c;
            lv->cellCount++;
        }
    }

    lv->goalMask = 0;
    lv->startMask = 0;
    lv->startPlayer = -1;
    for (int i = 0; i < lv->cellCount; i++)
    {
        int r = lv->cellRow[i];
        int c = lv->cellCol[i];
        char ch = BoardLevelRow(level, r)[c];

        if (ch == '.')
            lv->goalMask |= 1ULL << i;
        else if (ch == '$')
            lv->startMask |= 1ULL << i;
        else if (ch == '@')
            lv->startPlayer = i;

        for (int d = 0; d < 4; d++)
        {
            int nr = r + HINT_DIR[d][0];
            int nc = c + HINT_DIR[d][1];
            lv->neighbour[i][d] =
                (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS)
                    ? lv->cellAt[nr][nc] : -1;
        }
    }

    if (lv->startPlayer < 0)
        return false;

    return __builtin_popcountll(lv->goalMask) ==
           __builtin_popcountll(lv->startMask);
}

static void HintBuildRoute(HintLevel *lv, int level)
{
    lv->routeCount = 0;

    const CheatRoute *route = CheatsheetRoute(level);
    if (!route)
        return;

    uint64_t crates = lv->startMask;
    int at = lv->startPlayer;

    for (int s = 0; s < route->stepCount; s++)
    {
        int d = 3;
        if (route->step[s].direction == CHEAT_UP) d = 0;
        else if (route->step[s].direction == CHEAT_DOWN) d = 1;
        else if (route->step[s].direction == CHEAT_LEFT) d = 2;

        for (int k = 0; k < route->step[s].repeat; k++)
        {
            int ahead = lv->neighbour[at][d];
            if (ahead < 0)
                break;
            if (((crates >> ahead) & 1ULL) == 0)
            {
                at = ahead;
                continue;
            }

            int beyond = lv->neighbour[ahead][d];
            if (beyond < 0 || ((crates >> beyond) & 1ULL) != 0)
                break;

            crates = (crates & ~(1ULL << ahead)) | (1ULL << beyond);
            at = ahead;
            if (lv->routeCount < HINT_MAX_ROUTE)
            {
                lv->routeMask[lv->routeCount] = crates;
                lv->routePlayer[lv->routeCount] =
                    (uint16_t)HintNormalise(lv, crates, at);
                lv->routeCount++;
            }
        }
    }
}

static bool HintOnRoute(const HintLevel *lv, uint64_t mask, uint16_t player)
{
    for (int i = 0; i < lv->routeCount; i++)
        if (lv->routeMask[i] == mask && lv->routePlayer[i] == player)
            return true;
    return false;
}

typedef struct HintQueue {
    uint64_t *mask;
    uint16_t *player;
    size_t head;
    size_t tail;
    size_t room;
    bool failed;
} HintQueue;

static void HintQueuePush(HintQueue *q, uint64_t mask, int player)
{
    if (q->failed)
        return;
    if (q->tail == q->room)
    {
        size_t room = q->room ? q->room * 2 : 4096;
        uint64_t *m = realloc(q->mask, room * sizeof(uint64_t));
        if (m) q->mask = m;
        uint16_t *p = m ? realloc(q->player, room * sizeof(uint16_t)) : NULL;
        if (!m || !p)
        {
            q->failed = true;
            return;
        }
        q->player = p;
        q->room = room;
    }
    q->mask[q->tail] = mask;
    q->player[q->tail] = (uint16_t)player;
    q->tail++;
}

static bool HintBuildTable(HintLevel *lv, int level)
{
    if (!HintGeometry(lv, level))
        return false;
    HintBuildRoute(lv, level);

    HintQueue queue = {0};

    for (int i = 0; i < lv->cellCount; i++)
    {
        if (((lv->goalMask >> i) & 1ULL) != 0)
            continue;
        int at = HintNormalise(lv, lv->goalMask, i);
        if (HintInsert(lv, lv->goalMask, (uint16_t)at, 0))
            HintQueuePush(&queue, lv->goalMask, at);
    }

    while (queue.head < queue.tail && !queue.failed)
    {
        uint64_t crates = queue.mask[queue.head];
        int at = queue.player[queue.head];
        queue.head++;

        HintEntry *here = HintFind(lv, crates, (uint16_t)at);
        if (!here)
            continue;
        uint16_t next = (uint16_t)(here->dist + 1);
        uint64_t reach = HintReach(lv, crates, at);

        for (int i = 0; i < lv->cellCount; i++)
        {
            if (((crates >> i) & 1ULL) == 0)
                continue;

            for (int d = 0; d < 4; d++)
            {

                int behind = lv->neighbour[i][d ^ 1];
                if (behind < 0 || ((crates >> behind) & 1ULL) != 0)
                    continue;
                if (((reach >> behind) & 1ULL) == 0)
                    continue;
                int further = lv->neighbour[behind][d ^ 1];
                if (further < 0 || ((crates >> further) & 1ULL) != 0)
                    continue;

                uint64_t moved = (crates & ~(1ULL << i)) | (1ULL << behind);
                int at2 = HintNormalise(lv, moved, further);
                if (HintInsert(lv, moved, (uint16_t)at2, next))
                    HintQueuePush(&queue, moved, at2);
            }
        }
    }

    bool ok = !queue.failed;
    free(queue.mask);
    free(queue.player);
    return ok;
}

Hint HintAsk(const Board *board)
{
    Hint hint = {0};
    hint.status = HINT_IDLE;

    int level = board->currentLevel;
    if (level < 0 || level >= LEVEL_COUNT)
        return hint;

    HintLevel *lv = &hintLevels[level];
    if (!lv->attempted)
    {
        lv->attempted = true;
        lv->usable = HintBuildTable(lv, level);
    }
    if (!lv->usable)
        return hint;

    uint64_t crates = 0;
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if (board->boxes[r][c] && lv->cellAt[r][c] >= 0)
                crates |= 1ULL << lv->cellAt[r][c];

    if (crates == lv->goalMask)
    {
        hint.status = HINT_SOLVED;
        return hint;
    }

    int standing = lv->cellAt[board->playerRow][board->playerCol];
    if (standing < 0)
        return hint;
    int at = HintNormalise(lv, crates, standing);

    HintEntry *here = HintFind(lv, crates, (uint16_t)at);
    if (!here)
    {

        hint.status = HINT_DEAD;
        return hint;
    }

    hint.pushesLeft = here->dist;

    uint16_t want = (uint16_t)(here->dist - 1);
    uint64_t reach = HintReach(lv, crates, at);
    int bestBox = -1;
    int bestDir = -1;
    int bestStand = -1;
    bool bestOnRoute = false;

    for (int i = 0; i < lv->cellCount && !bestOnRoute; i++)
    {
        if (((crates >> i) & 1ULL) == 0)
            continue;

        for (int d = 0; d < 4; d++)
        {
            int from = lv->neighbour[i][d ^ 1];
            int to = lv->neighbour[i][d];
            if (from < 0 || to < 0)
                continue;
            if (((crates >> from) & 1ULL) != 0 || ((crates >> to) & 1ULL) != 0)
                continue;
            if (((reach >> from) & 1ULL) == 0)
                continue;

            uint64_t moved = (crates & ~(1ULL << i)) | (1ULL << to);

            int at2 = HintNormalise(lv, moved, i);
            HintEntry *there = HintFind(lv, moved, (uint16_t)at2);
            if (!there || there->dist != want)
                continue;

            bool onRoute = HintOnRoute(lv, moved, (uint16_t)at2);
            if (bestBox < 0 || (onRoute && !bestOnRoute))
            {
                bestBox = i;
                bestDir = d;
                bestStand = from;
                bestOnRoute = onRoute;
            }
            if (bestOnRoute)
                break;
        }
    }

    if (bestBox < 0)
        return hint;

    hint.status = HINT_READY;
    hint.boxRow = lv->cellRow[bestBox];
    hint.boxCol = lv->cellCol[bestBox];
    hint.pushRow = HINT_DIR[bestDir][0];
    hint.pushCol = HINT_DIR[bestDir][1];
    hint.standRow = lv->cellRow[bestStand];
    hint.standCol = lv->cellCol[bestStand];
    return hint;
}

void HintDraw(const Hint *hint, int originX, int originY, int tile)
{
    if (hint->status != HINT_READY)
        return;

    float pulse = 0.5f + 0.5f * sinf((float)GetTime() * 3.0f);
    float size = (float)tile;

    Rectangle crate = {
        (float)(originX + hint->boxCol * tile),
        (float)(originY + hint->boxRow * tile),
        size, size
    };
    Vector2 travel = {hint->pushCol * size, hint->pushRow * size};
    Rectangle target = {crate.x + travel.x, crate.y + travel.y, size, size};
    Vector2 stand = {
        crate.x - travel.x + size / 2.0f,
        crate.y - travel.y + size / 2.0f
    };

    DrawCircleLinesV(stand, size * 0.19f,
                     Fade(COL_NOIR_IVORY, 0.26f + 0.20f * pulse));

    DrawRectangleRec(target, Fade(COL_NOIR_GOLD, 0.09f + 0.09f * pulse));
    Vector2 head = {target.x + size / 2.0f, target.y + size / 2.0f};
    float turn = atan2f((float)hint->pushRow, (float)hint->pushCol) * RAD2DEG;
    DrawPoly(head, 3, size * 0.25f, turn, Fade(BLACK, 0.55f));
    DrawPoly(head, 3, size * 0.21f, turn,
             Fade(COL_NOIR_GOLD, 0.85f + 0.15f * pulse));

    float arm = size * 0.30f;
    float thick = size * 0.055f;
    if (thick < 2.0f)
        thick = 2.0f;
    Color mark = Fade(COL_NOIR_GOLD, 0.70f + 0.30f * pulse);
    float right = crate.x + size - thick;
    float bottom = crate.y + size - thick;

    DrawRectangleRec((Rectangle){crate.x, crate.y, arm, thick}, mark);
    DrawRectangleRec((Rectangle){crate.x, crate.y, thick, arm}, mark);
    DrawRectangleRec((Rectangle){crate.x + size - arm, crate.y, arm, thick}, mark);
    DrawRectangleRec((Rectangle){right, crate.y, thick, arm}, mark);
    DrawRectangleRec((Rectangle){crate.x, bottom, arm, thick}, mark);
    DrawRectangleRec((Rectangle){crate.x, crate.y + size - arm, thick, arm}, mark);
    DrawRectangleRec((Rectangle){crate.x + size - arm, bottom, arm, thick}, mark);
    DrawRectangleRec((Rectangle){right, crate.y + size - arm, thick, arm}, mark);
}

void HintDrawDeadNotice(Assets *asset, Rectangle area)
{
    float pulse = 0.5f + 0.5f * sinf((float)GetTime() * 2.2f);

    DrawPauseButtonBase(asset->texPauseButtonNormal, area, WHITE);
    DrawFontCenteredInRectangle(
        asset->fontNoir, "NO WAY OUT - UNDO OR RESTART", area,
        area.height * 0.30f, 1.0f,
        Fade(COL_NOIR_RED, 0.75f + 0.25f * pulse), 0);
}

void HintUnload(void)
{
    for (int i = 0; i < LEVEL_COUNT; i++)
    {
        free(hintLevels[i].slot);
        hintLevels[i].slot = NULL;
        hintLevels[i].capacity = 0;
        hintLevels[i].count = 0;
        hintLevels[i].attempted = false;
        hintLevels[i].usable = false;
    }
}

