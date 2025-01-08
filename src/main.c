#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "raylib.h"
#include "constants.h"

void setup();
void update();
void handlePlayerMoveInput();
bool wouldCollide(float newX, float newY);
void movePlayer(float deltaTime);
void render();
void drawMap();
void drawPlayer();

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

struct Player {
    float x;
    float y;
    float width;
    float height;
    int turnDirection; // -1 for left, 1 for right
    int walkDirection; // -1 for back, 1 for forwards
    float rotationAngle;
    float walkSpeed;
    float turnSpeed;
} gamePlayer;

int main() 
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Raycaster");

    HideCursor();
    DisableCursor();

    setup();

    while (!WindowShouldClose())
    {
        update();
        render();
    }

    CloseWindow();

    return 0;
}

void setup() {
    gamePlayer.x = WINDOW_WIDTH / 2;
    gamePlayer.y = WINDOW_HEIGHT / 2;
    gamePlayer.width = 5;
    gamePlayer.height = 5;
    gamePlayer.turnDirection = 0;
    gamePlayer.walkDirection = 0;
    gamePlayer.rotationAngle = PI / 2;
    gamePlayer.walkSpeed = 100;
    gamePlayer.turnSpeed = 45 * (PI / 180);
}

void update() {
    float deltaTime = GetFrameTime();
    handlePlayerMoveInput();
    movePlayer(deltaTime);
}

void drawMap() {
    for (int i=0; i < MAP_NUM_ROWS; i++) {
        for (int j=0; j < MAP_NUM_COLS; j++) {
            int tileX = j * TILE_SIZE;
            int tileY = i * TILE_SIZE;
            Color tileColour = map[i][j] != 0 ? RAYWHITE : BLACK;
            DrawRectangle(
                tileX * MINIMAP_SCALE_FACTOR, 
                tileY * MINIMAP_SCALE_FACTOR, 
                TILE_SIZE * MINIMAP_SCALE_FACTOR, 
                TILE_SIZE * MINIMAP_SCALE_FACTOR, 
                tileColour
            );
        }
    }
}

void handlePlayerMoveInput() {
    if (IsKeyUp(KEY_UP)) {
        gamePlayer.walkDirection = 0;
    }
    if (IsKeyUp(KEY_DOWN)) {
        gamePlayer.walkDirection = 0;
    }
    if (IsKeyUp(KEY_LEFT)) {
        gamePlayer.turnDirection = 0;
    }
    if (IsKeyUp(KEY_RIGHT)) {
        gamePlayer.turnDirection = 0;
    }
    if (IsKeyDown(KEY_UP)) {
        gamePlayer.walkDirection = +1;
    }
    if (IsKeyDown(KEY_DOWN)) {
        gamePlayer.walkDirection = -1;
    }
    if (IsKeyDown(KEY_LEFT)) {
        gamePlayer.turnDirection = -1;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        gamePlayer.turnDirection = +1;
    }
}

void movePlayer(float deltaTime) {
    gamePlayer.rotationAngle += gamePlayer.turnDirection * gamePlayer.turnSpeed * deltaTime;
    float moveStep = gamePlayer.walkDirection * gamePlayer.walkSpeed * deltaTime;
    
    float newPlayerX = gamePlayer.x + cos(gamePlayer.rotationAngle) * moveStep;
    float newPlayerY = gamePlayer.y + sin(gamePlayer.rotationAngle) * moveStep;

    if (!wouldCollide(newPlayerX, newPlayerY)) {
        gamePlayer.x = newPlayerX;
        gamePlayer.y = newPlayerY;
    }
}

bool wouldCollide(float newX, float newY) {
    if (newX < 0 || newX > WINDOW_WIDTH || newY < 0 || newY > WINDOW_HEIGHT) {
        return true;
    }

    int mapXIdx = floor(newX / TILE_SIZE);
    int mapYIdx = floor(newY / TILE_SIZE);
    return map[mapYIdx][mapXIdx];
}

void drawPlayer() {
    DrawRectangle(
        gamePlayer.x * MINIMAP_SCALE_FACTOR, 
        gamePlayer.y * MINIMAP_SCALE_FACTOR, 
        gamePlayer.width * MINIMAP_SCALE_FACTOR, 
        gamePlayer.height * MINIMAP_SCALE_FACTOR,
        YELLOW
    );
    DrawLine(
        gamePlayer.x + 2.5 * MINIMAP_SCALE_FACTOR, 
        gamePlayer.y * MINIMAP_SCALE_FACTOR,
        gamePlayer.x + 2.5 + cos(gamePlayer.rotationAngle) * 40,
        gamePlayer.y + sin(gamePlayer.rotationAngle) * 40,
        YELLOW
    );
}

void render() {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        drawMap();
        //drawRays();
        drawPlayer();
    EndDrawing();
}