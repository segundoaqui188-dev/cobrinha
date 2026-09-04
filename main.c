#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define GRID_COLUMNS 20
#define GRID_ROWS 28
#define MAX_SNAKE_LENGTH (GRID_COLUMNS * GRID_ROWS)

typedef struct { int x; int y; } Cell;
typedef enum { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT } Direction;

static Cell snake[MAX_SNAKE_LENGTH];
static int snakeLength = 4;
static Cell food;
static Direction currentDirection = DIR_RIGHT;
static Direction nextDirection = DIR_RIGHT;
static int score = 0;
static bool gameOver = false;
static float moveTimer = 0.0f;
static float moveInterval = 0.11f;

static bool PointInRect(Rectangle r, Vector2 p) { return CheckCollisionPointRec(p, r); }

static void SpawnFood(void) {
    bool valid = false;
    while (!valid) {
        food.x = GetRandomValue(0, GRID_COLUMNS - 1);
        food.y = GetRandomValue(0, GRID_ROWS - 1);
        valid = true;
        for (int i = 0; i < snakeLength; ++i) {
            if (snake[i].x == food.x && snake[i].y == food.y) { valid = false; break; }
        }
    }
}

static void ResetGame(void) {
    snakeLength = 4;
    score = 0;
    currentDirection = DIR_RIGHT;
    nextDirection = DIR_RIGHT;
    moveTimer = 0.0f;
    moveInterval = 0.11f;
    gameOver = false;
    int sx = GRID_COLUMNS / 2;
    int sy = GRID_ROWS / 2;
    for (int i = 0; i < snakeLength; ++i) {
        snake[i].x = sx - i;
        snake[i].y = sy;
    }
    SpawnFood();
}

static bool Opposite(Direction a, Direction b) {
    return (a == DIR_UP && b == DIR_DOWN) ||
           (a == DIR_DOWN && b == DIR_UP) ||
           (a == DIR_LEFT && b == DIR_RIGHT) ||
           (a == DIR_RIGHT && b == DIR_LEFT);
}

static void SetDirection(Direction d) {
    if (!Opposite(currentDirection, d)) nextDirection = d;
}

static void MoveSnake(void) {
    if (gameOver) return;
    currentDirection = nextDirection;
    Cell head = snake[0];
    if (currentDirection == DIR_UP) --head.y;
    if (currentDirection == DIR_DOWN) ++head.y;
    if (currentDirection == DIR_LEFT) --head.x;
    if (currentDirection == DIR_RIGHT) ++head.x;

    if (head.x < 0 || head.x >= GRID_COLUMNS || head.y < 0 || head.y >= GRID_ROWS) {
        gameOver = true;
        return;
    }

    for (int i = 0; i < snakeLength; ++i) {
        if (head.x == snake[i].x && head.y == snake[i].y) {
            gameOver = true;
            return;
        }
    }

    bool ate = (head.x == food.x && head.y == food.y);
    if (ate && snakeLength < MAX_SNAKE_LENGTH) {
        ++snakeLength;
        ++score;
        if ((moveInterval -= 0.002f) < 0.055f) moveInterval = 0.055f;
    }

    for (int i = snakeLength - 1; i > 0; --i) snake[i] = snake[i - 1];
    snake[0] = head;
    if (ate) SpawnFood();
}

static void HandleKeyboard(void) {
    if (IsKeyPressed(KEY_UP)) SetDirection(DIR_UP);
    if (IsKeyPressed(KEY_DOWN)) SetDirection(DIR_DOWN);
    if (IsKeyPressed(KEY_LEFT)) SetDirection(DIR_LEFT);
    if (IsKeyPressed(KEY_RIGHT)) SetDirection(DIR_RIGHT);
    if (IsKeyPressed(KEY_R)) ResetGame();
}

static void HandleInput(Rectangle up, Rectangle down, Rectangle left, Rectangle right) {
    int touches = GetTouchPointCount();
    for (int i = 0; i < touches; ++i) {
        Vector2 p = GetTouchPosition(i);
        if (PointInRect(up, p)) SetDirection(DIR_UP);
        else if (PointInRect(down, p)) SetDirection(DIR_DOWN);
        else if (PointInRect(left, p)) SetDirection(DIR_LEFT);
        else if (PointInRect(right, p)) SetDirection(DIR_RIGHT);
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 p = GetMousePosition();
        if (PointInRect(up, p)) SetDirection(DIR_UP);
        else if (PointInRect(down, p)) SetDirection(DIR_DOWN);
        else if (PointInRect(left, p)) SetDirection(DIR_LEFT);
        else if (PointInRect(right, p)) SetDirection(DIR_RIGHT);
    }
}

static void ArrowButton(Rectangle r, const char *text, bool pressed) {
    DrawRectangleRounded(r, 0.2f, 8, pressed ? DARKGRAY : GRAY);
    DrawRectangleRoundedLines(r, 0.2f, 8, 2.0f, LIGHTGRAY);
    int size = (int)(r.height * 0.42f);
    int width = MeasureText(text, size);
    DrawText(text, (int)(r.x + (r.width - width) * 0.5f),
             (int)(r.y + (r.height - size) * 0.5f), size, WHITE);
}

static void DrawGame(float width, float height, float controlsTop) {
    ClearBackground((Color){ 22, 42, 28, 255 });
    float cw = width / GRID_COLUMNS;
    float ch = (controlsTop - 4.0f) / GRID_ROWS;

    for (int y = 0; y <= GRID_ROWS; ++y) DrawLine(0, (int)(y * ch), (int)width, (int)(y * ch), (Color){ 32, 58, 38, 255 });
    for (int x = 0; x <= GRID_COLUMNS; ++x) DrawLine((int)(x * cw), 0, (int)(x * cw), (int)controlsTop, (Color){ 32, 58, 38, 255 });

    Rectangle f = { food.x * cw + cw * 0.12f, food.y * ch + ch * 0.12f, cw * 0.76f, ch * 0.76f };
    DrawRectangleRounded(f, 0.35f, 8, RED);

    for (int i = snakeLength - 1; i >= 0; --i) {
        Rectangle s = { snake[i].x * cw + cw * 0.10f, snake[i].y * ch + ch * 0.10f, cw * 0.80f, ch * 0.80f };
        DrawRectangleRounded(s, 0.25f, 8, i == 0 ? LIME : GREEN);
    }

    DrawRectangle(0, (int)controlsTop, (int)width, (int)(height - controlsTop), (Color){ 15, 34, 20, 255 });
    DrawText(TextFormat("SCORE: %d", score), 14, 12, 24, WHITE);
}

static void DrawControls(float width, float height, Rectangle up, Rectangle down, Rectangle left, Rectangle right) {
    Vector2 p = GetTouchPosition(0);
    bool touching = GetTouchPointCount() > 0;
    ArrowButton(up, "^", touching && PointInRect(up, p));
    ArrowButton(down, "v", touching && PointInRect(down, p));
    ArrowButton(left, "<", touching && PointInRect(left, p));
    ArrowButton(right, ">", touching && PointInRect(right, p));

    if (gameOver) {
        DrawRectangle(0, 0, (int)width, (int)height, (Color){ 0, 0, 0, 150 });
        const char *title = "GAME OVER";
        const char *hint = "TOQUE NA TELA OU PRESSIONE R";
        int ts = 38, hs = 17;
        DrawText(title, (int)(width / 2 - MeasureText(title, ts) / 2), (int)(height * 0.35f), ts, RED);
        DrawText(hint, (int)(width / 2 - MeasureText(hint, hs) / 2), (int)(height * 0.44f), hs, WHITE);
        Rectangle r = { width / 2 - 100, height * 0.52f, 200, 56 };
        DrawRectangleRounded(r, 0.20f, 8, DARKGREEN);
        const char *label = "REINICIAR";
        DrawText(label, (int)(r.x + (r.width - MeasureText(label, 22)) / 2), (int)(r.y + 16), 22, WHITE);

        Vector2 mouse = GetMousePosition();
        if (IsKeyPressed(KEY_R) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || GetTouchPointCount() > 0) {
            if (IsKeyPressed(KEY_R) || PointInRect(r, mouse) || GetTouchPointCount() > 0) ResetGame();
        }
    }
}

int main(void) {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(800, 900, "Snake Raylib Android");
    SetTargetFPS(60);
    SetRandomSeed((unsigned int)time(NULL));
    ResetGame();

    while (!WindowShouldClose()) {
        float w = (float)GetScreenWidth();
        float h = (float)GetScreenHeight();
        float controlsHeight = h * 0.25f;
        if (controlsHeight < 190.0f) controlsHeight = 190.0f;
        if (controlsHeight > 280.0f) controlsHeight = 280.0f;
        float top = h - controlsHeight;
        float size = controlsHeight * 0.30f;
        if (size < 58.0f) size = 58.0f;
        if (size > 100.0f) size = 100.0f;
        float cx = w * 0.5f;

        Rectangle up = { cx - size * 0.5f, top + 12, size, size };
        Rectangle down = { cx - size * 0.5f, h - size - 14, size, size };
        Rectangle left = { cx - size - 12, h - size - 14, size, size };
        Rectangle right = { cx + 12, h - size - 14, size, size };

        HandleKeyboard();
        if (!gameOver) HandleInput(up, down, left, right);

        if (!gameOver) {
            moveTimer += GetFrameTime();
            if (moveTimer >= moveInterval) { moveTimer = 0.0f; MoveSnake(); }
        }

        BeginDrawing();
        DrawGame(w, h, top);
        DrawControls(w, h, up, down, left, right);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
