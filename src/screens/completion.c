#include "completion.h"

#include "audio.h"
#include "screen.h"
#include "score.h"
#include "ui.h"

CompletionLayout CompletionGetLayout(void)
{
    CompletionLayout layout = {0};
    layout.panel = (Rectangle){
        SCREEN_W * 0.510f, SCREEN_H * 0.020f,
        SCREEN_W * 0.476f, SCREEN_H * 0.885f
    };

    float nextWidth = layout.panel.width * 0.627f;
    float smallWidth = layout.panel.width * 0.565f;
    float nextHeight = layout.panel.height * 0.114f;
    float smallHeight = layout.panel.height * 0.071f;
    float gap = layout.panel.height * 0.013f;
    float buttonY = layout.panel.y + layout.panel.height * 0.693f;
    float centerX = layout.panel.x + layout.panel.width / 2.0f;

    layout.nextButton = (Rectangle){
        centerX - nextWidth / 2.0f, buttonY, nextWidth, nextHeight
    };
    layout.replayButton = (Rectangle){
        centerX - smallWidth / 2.0f, buttonY + nextHeight + gap,
        smallWidth, smallHeight
    };
    layout.homeButton = (Rectangle){
        centerX - smallWidth / 2.0f,
        layout.replayButton.y + smallHeight + gap,
        smallWidth, smallHeight
    };
    return layout;
}

static void DrawStar(Texture2D star, Rectangle ink, Rectangle box, Color tint)
{
    float scale = box.width / ink.width;
    if (box.height / ink.height < scale)
        scale = box.height / ink.height;

    float width = ink.width * scale;
    float height = ink.height * scale;
    Rectangle destination = {
        box.x + (box.width - width) / 2.0f,
        box.y + (box.height - height) / 2.0f,
        width, height
    };
    DrawTexturePro(star, ink, destination, (Vector2){0, 0}, 0, tint);
}

#define STAR_REVEAL_FIRST 0.38f
#define STAR_REVEAL_STEP  0.30f
#define STAR_REVEAL_POP   0.24f

static const float STAR_TICK_PITCH[3] = {1.0000f, 1.2599f, 1.4983f};

static void DrawCompletionStars(Assets *asset, Rectangle panel,
                                int starRating, float elapsedTime)
{
    Rectangle filledInk = PauseInk(asset->texStarFilled,
                                   0.049f, 0.053f, 0.900f, 0.829f);
    Rectangle emptyInk = PauseInk(asset->texStarEmpty,
                                  0.022f, 0.021f, 0.955f, 0.936f);

    float starSize = panel.width * 0.208f;
    float gap = panel.width * 0.024f;
    float totalWidth = starSize * 3.0f + gap * 2.0f;
    float startX = panel.x + (panel.width - totalWidth) / 2.0f;
    float y = panel.y + panel.height * 0.246f;

    for (int i = 0; i < 3; i++)
    {
        Rectangle box = {
            startX + i * (starSize + gap), y, starSize, starSize
        };

        float animationProgress = (i < starRating)
            ? (elapsedTime - (STAR_REVEAL_FIRST + i * STAR_REVEAL_STEP))
              / STAR_REVEAL_POP
            : 0.0f;
        if (animationProgress > 1.0f)
            animationProgress = 1.0f;

        if (i >= starRating || animationProgress < 1.0f)
            DrawStar(asset->texStarEmpty, emptyInk, box, WHITE);

        if (i >= starRating || animationProgress <= 0.0f)
            continue;

        float remaining = 1.0f - animationProgress;
        float scale = 1.0f + 0.85f * remaining * remaining;

        float fade = animationProgress * 2.2f;
        if (fade > 1.0f)
            fade = 1.0f;

        float grow = starSize * (scale - 1.0f) / 2.0f;
        Rectangle popped = {
            box.x - grow, box.y - grow,
            box.width + grow * 2.0f, box.height + grow * 2.0f
        };
        DrawStar(asset->texStarFilled, filledInk, popped, Fade(WHITE, fade));
    }
}

static void DrawCompletionRow(Font font, const char *label,
                              const char *value, float left, float right,
                              float y, float fontSize)
{
    Color ink = (Color){31, 27, 24, 255};
    DrawTextEx(font, label, (Vector2){left, y}, fontSize, -0.4f, ink);
    Vector2 valueSize = MeasureTextEx(font, value, fontSize, -0.4f);
    DrawTextEx(font, value, (Vector2){right - valueSize.x, y},
               fontSize, -0.4f, ink);
}

#define COMPLETION_PHOTO_SHIFT (-0.1336f)

void CompletionDraw(const Board *board, Assets *asset,
                    CompletionLayout layout,
                    bool nextHovered, bool replayHovered,
                    bool homeHovered, float elapsedTime)
{

    float photoScale = (float)SCREEN_H / asset->texCompletion.height;
    Rectangle source = {
        0, 0,
        (float)asset->texCompletion.width,
        (float)asset->texCompletion.height
    };
    Rectangle photoDestination = {
        COMPLETION_PHOTO_SHIFT * SCREEN_W, 0,
        asset->texCompletion.width * photoScale, SCREEN_H
    };
    Rectangle screen = {0, 0, SCREEN_W, SCREEN_H};

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, BLACK);
    DrawTexturePro(asset->texCompletion, source, photoDestination,
                   (Vector2){0, 0}, 0, WHITE);

    float photoRight = photoDestination.x + photoDestination.width;
    if (photoRight < SCREEN_W)
    {
        float band = 160.0f;
        Rectangle edgeSource = {
            asset->texCompletion.width - band, 0,
            band, (float)asset->texCompletion.height
        };

        Rectangle edgeDestination = {
            photoRight - 1.0f, 0, SCREEN_W - photoRight + 1.0f, SCREEN_H
        };
        DrawTexturePro(asset->texCompletion, edgeSource, edgeDestination,
                       (Vector2){0, 0}, 0, WHITE);
    }
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.18f));

    Rectangle overlaySource = {
        0, 0,
        asset->texCompletionEdgeOverlay.width,
        asset->texCompletionEdgeOverlay.height
    };
    DrawTexturePro(asset->texCompletionEdgeOverlay, overlaySource, screen,
                   (Vector2){0, 0}, 0, WHITE);

    Rectangle panelSource = {
        0, 0, asset->texPausePanel.width, asset->texPausePanel.height
    };
    DrawTexturePro(asset->texPausePanel, panelSource, layout.panel,
                   (Vector2){0, 0}, 0, WHITE);

    Rectangle titleInk = PauseInk(asset->texCompletionTitle,
                                  0.046f, 0.221f, 0.908f, 0.535f);
    float titleWidth = layout.panel.width * 0.600f;
    float titleHeight = titleWidth * titleInk.height / titleInk.width;
    Rectangle titleDestination = {
        layout.panel.x + (layout.panel.width - titleWidth) / 2.0f,
        layout.panel.y + layout.panel.height * 0.085f,
        titleWidth, titleHeight
    };
    DrawTexturePro(asset->texCompletionTitle, titleInk, titleDestination,
                   (Vector2){0, 0}, 0, WHITE);

    Rectangle levelArea = {
        layout.panel.x + layout.panel.width * 0.12f,
        layout.panel.y + layout.panel.height * 0.196f,
        layout.panel.width * 0.76f,
        layout.panel.height * 0.052f
    };
    DrawFontCenteredInRectangle(
        asset->fontNoir,
        TextFormat("LEVEL %i COMPLETE", board->currentLevel + 1),
        levelArea, layout.panel.height * 0.040f, 1.0f,
        (Color){34, 29, 25, 255}, 0
    );

    int starRating = GetCompletionStarRating(board);
    DrawCompletionStars(asset, layout.panel, starRating, elapsedTime);

    float ruleLeft = layout.panel.x + layout.panel.width * 0.165f;
    float ruleRight = layout.panel.x + layout.panel.width * 0.789f;
    float rowLeft = layout.panel.x + layout.panel.width * 0.186f;
    float rowRight = layout.panel.x + layout.panel.width * 0.650f;
    float rowY = layout.panel.y + layout.panel.height * 0.470f;
    float rowStep = layout.panel.height * 0.0380f;
    float rowFont = layout.panel.height * 0.029f;
    Color ruleColor = (Color){45, 39, 34, 190};

    int pushScore = GetPushScore(board);
    int moveBonus = GetMoveBonus(board);
    char pushValue[24];
    char minimumValue[24];
    char moveValue[24];
    char bonusValue[24];
    char scoreValue[24];
    TextCopy(pushValue, TextFormat("%i", board->pushCount));
    TextCopy(minimumValue,
             TextFormat("%i", LevelMinimumPushes(board->currentLevel)));
    TextCopy(moveValue, TextFormat("%i", board->moveCount));
    FormatScore(bonusValue, moveBonus);
    TextCopy(bonusValue, TextFormat("+%s", bonusValue));
    FormatScore(scoreValue, pushScore + moveBonus);

    const char *rowLabels[4] = {"PUSHES", "MINIMUM", "MOVES", "MOVE BONUS"};
    const char *rowValues[4] = {
        pushValue, minimumValue, moveValue, bonusValue
    };

    DrawLineEx((Vector2){ruleLeft, rowY - rowStep * 0.30f},
               (Vector2){ruleRight, rowY - rowStep * 0.30f},
               2.5f, ruleColor);
    for (int i = 0; i < 4; i++)
    {
        float y = rowY + rowStep * i;
        DrawCompletionRow(asset->fontNoir, rowLabels[i], rowValues[i],
                          rowLeft, rowRight, y, rowFont);

        if (i < 3)
            DrawLineEx((Vector2){ruleLeft, y + rowStep * 0.84f},
                       (Vector2){ruleRight, y + rowStep * 0.84f},
                       1.0f, Fade(ruleColor, 0.55f));
    }

    DrawLineEx((Vector2){ruleLeft, layout.panel.y + layout.panel.height * 0.618f},
               (Vector2){ruleRight, layout.panel.y + layout.panel.height * 0.618f},
               2.5f, ruleColor);
    DrawCompletionRow(asset->fontNoir, "LEVEL SCORE", scoreValue,
                      rowLeft, rowRight,
                      layout.panel.y + layout.panel.height * 0.630f,
                      layout.panel.height * 0.038f);

    if (ScoreIsNewBest())
    {
        Rectangle stampInk = PauseInk(asset->texCompletionNewBest,
                                      0.044f, 0.154f, 0.924f, 0.710f);
        float stampWidth = layout.panel.width * 0.205f;
        float stampHeight = stampWidth * stampInk.height / stampInk.width;
        Rectangle stampDestination = {
            layout.panel.x + layout.panel.width * 0.685f,
            layout.panel.y + layout.panel.height * 0.492f,
            stampWidth, stampHeight
        };
        DrawTexturePro(asset->texCompletionNewBest, stampInk, stampDestination,
                       (Vector2){0, 0}, 0, WHITE);
    }

    Rectangle nextInk = PauseInk(asset->texCompletionNextLabel,
                                 0.065f, 0.280f, 0.869f, 0.428f);
    Rectangle replayInk = PauseInk(asset->texCompletionReplayLabel,
                                   0.130f, 0.258f, 0.748f, 0.483f);
    Rectangle homeInk = PauseInk(asset->texPauseTextHome,
                                 0.082f, 0.271f, 0.714f, 0.414f);

    DrawPauseMenuButton(asset, layout.nextButton,
                        asset->texPauseIconContinue,
                        asset->texCompletionNextLabel, nextInk, 0,
                        nextHovered, true);
    DrawPauseMenuButton(asset, layout.replayButton,
                        asset->texPauseIconRestart,
                        asset->texCompletionReplayLabel, replayInk, 1,
                        replayHovered, true);
    DrawPauseMenuButton(asset, layout.homeButton,
                        asset->texPauseIconHome,
                        asset->texPauseTextHome, homeInk, 3,
                        homeHovered, true);

    DrawFontCenteredInRectangle(
        asset->fontNoir, "MUSIC MUFFLED",
        (Rectangle){SCREEN_W * 0.845f, SCREEN_H * 0.940f,
                    SCREEN_W * 0.145f, SCREEN_H * 0.040f},
        SCREEN_H * 0.020f, 0.5f,
        (Color){210, 201, 186, 220}, 0
    );
}

void CompletionPlayStarTicks(const Board *board, float elapsedTime,
                             int *playedStarSounds)
{
    int earned = GetCompletionStarRating(board);

    while (*playedStarSounds < earned &&
           elapsedTime >= STAR_REVEAL_FIRST +
               *playedStarSounds * STAR_REVEAL_STEP)
    {
        AudioPlayPitched(SFX_STAR, STAR_TICK_PITCH[*playedStarSounds]);
        (*playedStarSounds)++;
    }
}
