#ifndef BOARD_H
#define BOARD_H

#include "raylib.h"
#include "assets.h"
#include "audio.h"

#define ROWS 10
#define COLS 15
#define LEVEL_COUNT 48

#define FLOOR 0
#define WALL 1
#define TARGET 2
#define VOID 3

#define UNDO_LIMIT 4096

typedef struct{
    int playerRow;
    int playerCol;
    int faceRow;
    int faceCol;
    int boxFromRow;
    int boxFromCol;
    int boxToRow;
    int boxToCol;
}BoardMove;

typedef struct{

    int map[ROWS][COLS];

    int boxes[ROWS][COLS];

    int playerRow;
    int playerCol;
    int faceRow;
    int faceCol;

    int currentLevel;

    int moveCount;
    int pushCount;
    int levelSolved;
    float push_pose_timer;
    float moveTimer;

    BoardMove history[UNDO_LIMIT];
    int historyCount;

}Board;

void BoardInit(Board *board);
void BoardLoadLevel(Board *board, int levelIndex);
void BoardUpdate(Board *board, float deltaTime);

void BoardSetMovement(int holdToRepeat, float repeatDelay);

int BoardUndo(Board *board);
void BoardDraw(const Board *board,Assets *assets, int originX, int originY, int tile);

const char *BoardLevelRow(int levelIndex, int row);

#endif
