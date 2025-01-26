#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "textures.h"
#include "constants.h"
#include "graphics.h"
#include "map.h"
#include "ray.h"
#include "player.h"

void setup();
void update();
void render();
void drawToColourBuffer();
void create3DProjection();
void releaseResources();

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
    initColourBuffer();
    loadWallTextures();
}

void update() {
    float deltaTime = GetFrameTime();
    handlePlayerMoveInput();
    movePlayer(deltaTime);
    castRays();
}

void releaseResources() {
    freeWallTextures();
    freeMemory();
}

void render() {
    BeginDrawing();
        ClearBackground(RAYWHITE);

        create3DProjection();

        drawToColourBuffer();

        drawMap();
        drawRays();
        drawPlayer();
    EndDrawing();
}