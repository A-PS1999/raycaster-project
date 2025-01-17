#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <float.h>
#include "textures.h"
#include "raymath.h"
#include "constants.h"

void setup();
void update();
void handlePlayerMoveInput();
bool wouldCollide(float newX, float newY);
void movePlayer(float deltaTime);
void castRays();
void castRay(float rayAngle, int stripId);
float normalizeAngle(float rayAngle);
bool hasWallAtPos(Vector2 mapPos);
void render();
void floodColourBuffer(Color colour);
void drawToColourBuffer();
void create3DProjection();
void drawMap();
void drawRays();
void drawPlayer();
void freeMemory();

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 2, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5}
};

struct Player {
    Vector2 playerPos;
    float width;
    float height;
    int turnDirection; // -1 for left, 1 for right
    int walkDirection; // -1 for back, 1 for forwards
    float rotationAngle;
    float walkSpeed;
    float turnSpeed;
} gamePlayer;

struct MyRay {
    float distance;
    float rayAngle;
    Vector2 wallHitPos;
    bool wasHitVertical;
    bool rayFacings[4]; // up, down, left, right
    int wallHitContent;
} rays[NUM_RAYS];

Color* colourBuffer = NULL;
Image imgFromBuffer;
Texture2D colourBufferTexture;
float distToProjectionPlane;

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
    UnloadTexture(colourBufferTexture);
    UnloadImage(imgFromBuffer);

    return 0;
}

void setup() {
    distToProjectionPlane = PROJ_PLANE_DIST;

    gamePlayer.playerPos.x = WINDOW_WIDTH / 2;
    gamePlayer.playerPos.y = WINDOW_HEIGHT / 2;
    gamePlayer.width = 5;
    gamePlayer.height = 5;
    gamePlayer.turnDirection = 0;
    gamePlayer.walkDirection = 0;
    gamePlayer.rotationAngle = PI / 2;
    gamePlayer.walkSpeed = 100;
    gamePlayer.turnSpeed = 45 * (PI / 180);

    colourBuffer = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(Color));
    imgFromBuffer = (Image){ 
        .data = colourBuffer, 
        .width = WINDOW_WIDTH, 
        .height = WINDOW_HEIGHT,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };
    colourBufferTexture = LoadTextureFromImage(imgFromBuffer);

    loadWallTextures();
}

void update() {
    float deltaTime = GetFrameTime();
    handlePlayerMoveInput();
    movePlayer(deltaTime);
    castRays();
}

void freeMemory() {
    free(colourBuffer);
    UnloadTexture(colourBufferTexture);
    UnloadImage(imgFromBuffer);
    freeWallTextures();
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
    
    float newPlayerX = gamePlayer.playerPos.x + cos(gamePlayer.rotationAngle) * moveStep;
    float newPlayerY = gamePlayer.playerPos.y + sin(gamePlayer.rotationAngle) * moveStep;

    if (!wouldCollide(newPlayerX, newPlayerY)) {
        gamePlayer.playerPos.x = newPlayerX;
        gamePlayer.playerPos.y = newPlayerY;
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

void castRays() {
    for (int col=0; col < NUM_RAYS; col++) {
        float rayAngle = gamePlayer.rotationAngle + atan2((col - NUM_RAYS/2), distToProjectionPlane);
        castRay(rayAngle, col);
    }
}

float normalizeAngle(float rayAngle) {
    rayAngle = remainder(rayAngle, TWO_PI);

    if (rayAngle < 0) {
        rayAngle = TWO_PI + rayAngle;
    }

    return rayAngle;
}

void castRay(float rayAngle, int stripId) {
    rayAngle = normalizeAngle(rayAngle);

    bool isRayFacingDown = rayAngle > 0 && rayAngle < PI;
    bool isRayFacingRight = rayAngle < (PI * 0.5) || rayAngle > (1.5 * PI);

    float xIntercept, yIntercept;
    float xStep, yStep;

    // Horizontal intersection
    bool foundHorizontalWall = false;
    int horizontalWallHitX = 0;
    int horizontalWallHitY = 0;
    int horizontalWallContent = 0;

    yIntercept = floor(gamePlayer.playerPos.y / TILE_SIZE) * TILE_SIZE;
    yIntercept += isRayFacingDown ? TILE_SIZE : 0;

    xIntercept = gamePlayer.playerPos.x + (yIntercept - gamePlayer.playerPos.y) / tan(rayAngle);

    yStep = TILE_SIZE;
    yStep *= !isRayFacingDown ? -1 : 1;

    xStep = TILE_SIZE / tan(rayAngle);
    xStep *= (!isRayFacingRight && xStep > 0) ? -1 : 1;
    xStep *= (isRayFacingRight && xStep < 0) ? -1 : 1;

    float nextHorizontalTouchX = xIntercept;
    float nextHorizontalTouchY = yIntercept;

    while (nextHorizontalTouchX >= 0 && nextHorizontalTouchX <= WINDOW_WIDTH && nextHorizontalTouchY >= 0 && nextHorizontalTouchY <= WINDOW_HEIGHT) {
        float xToCheck = nextHorizontalTouchX;
        float yToCheck = nextHorizontalTouchY + (!isRayFacingDown ? -1 : 0);

        if (hasWallAtPos((Vector2){xToCheck, yToCheck})) {
            horizontalWallHitX = nextHorizontalTouchX;
            horizontalWallHitY = nextHorizontalTouchY;
            horizontalWallContent = map[(int)floor(yToCheck/TILE_SIZE)][(int)floor(xToCheck/TILE_SIZE)];
            foundHorizontalWall = true;
            break;
        } else {
            nextHorizontalTouchX += xStep;
            nextHorizontalTouchY += yStep;
        }
    }

    // Vertical intersection
    bool foundVerticalWall = false;
    int verticalWallHitX = 0;
    int verticalWallHitY = 0;
    int verticalWallContent = 0;

    xIntercept = floor(gamePlayer.playerPos.x / TILE_SIZE) * TILE_SIZE;
    xIntercept += isRayFacingRight ? TILE_SIZE : 0;

    yIntercept = gamePlayer.playerPos.y + (xIntercept - gamePlayer.playerPos.x) * tan(rayAngle);

    xStep = TILE_SIZE;
    xStep *= !isRayFacingRight ? -1 : 1;

    yStep = TILE_SIZE * tan(rayAngle);
    yStep *= (!isRayFacingDown && yStep > 0) ? -1 : 1;
    yStep *= (isRayFacingDown && yStep < 0) ? -1 : 1;

    float nextVerticalTouchX = xIntercept;
    float nextVerticalTouchY = yIntercept;

    while (nextVerticalTouchX >= 0 && nextVerticalTouchX <= WINDOW_WIDTH && nextVerticalTouchY >= 0 && nextVerticalTouchY <= WINDOW_HEIGHT) {
        float xToCheck = nextVerticalTouchX + (!isRayFacingRight ? -1 : 0);
        float yToCheck = nextVerticalTouchY;

        if (hasWallAtPos((Vector2){xToCheck, yToCheck})) {
            verticalWallHitX = nextVerticalTouchX;
            verticalWallHitY = nextVerticalTouchY;
            verticalWallContent = map[(int)floor(yToCheck/TILE_SIZE)][(int)floor(xToCheck/TILE_SIZE)];
            foundVerticalWall = true;
            break;
        } else {
            nextVerticalTouchX += xStep;
            nextVerticalTouchY += yStep;
        }
    }

    float horizontalHitDistance = foundHorizontalWall ? Vector2Distance(gamePlayer.playerPos, (Vector2){horizontalWallHitX, horizontalWallHitY}) : FLT_MAX;
    float verticalHitDistance = foundVerticalWall ? Vector2Distance(gamePlayer.playerPos, (Vector2){verticalWallHitX, verticalWallHitY}) : FLT_MAX;

    if (verticalHitDistance < horizontalHitDistance) {
        rays[stripId].distance = verticalHitDistance;
        rays[stripId].wallHitPos = (Vector2){verticalWallHitX, verticalWallHitY};
        rays[stripId].wasHitVertical = true;
        rays[stripId].wallHitContent = verticalWallContent;
    } else {
        rays[stripId].distance = horizontalHitDistance;
        rays[stripId].wallHitPos = (Vector2){horizontalWallHitX, horizontalWallHitY};
        rays[stripId].wasHitVertical = false;
        rays[stripId].wallHitContent = horizontalWallContent;
    }
    rays[stripId].rayAngle = rayAngle;
    rays[stripId].rayFacings[0] = !isRayFacingDown;
    rays[stripId].rayFacings[1] = isRayFacingDown;
    rays[stripId].rayFacings[2] = !isRayFacingRight;
    rays[stripId].rayFacings[3] = isRayFacingRight;
}

void create3DProjection() {
    float distToProjectionPlane = (WINDOW_WIDTH/2) / tan(FOV_ANGLE/2);

    for (int i=0; i < NUM_RAYS; i++) {
        float perpenDistance = rays[i].distance * cos(rays[i].rayAngle - gamePlayer.rotationAngle);
        float projectedWallHeight = (TILE_SIZE / perpenDistance) * distToProjectionPlane;

        float wallTopPixel = (WINDOW_HEIGHT/2) - (projectedWallHeight/2);
        wallTopPixel = wallTopPixel < 0 ? 0 : wallTopPixel;

        float wallBottomPixel = (WINDOW_HEIGHT/2) + (projectedWallHeight/2);
        wallBottomPixel = wallBottomPixel > WINDOW_HEIGHT ? WINDOW_HEIGHT : wallBottomPixel;

        for (int ceilPixel=0; ceilPixel < wallTopPixel; ceilPixel++) {
            colourBuffer[(WINDOW_WIDTH * ceilPixel) + i] = (Color){ .a = 0xFF, .r = 0x8A, .g = 0x89, .b = 0x88 };
        }

        int textureOffsetX;

        if (rays[i].wasHitVertical) {
            textureOffsetX = (int)rays[i].wallHitPos.y % TILE_SIZE;
        } else {
            textureOffsetX = (int)rays[i].wallHitPos.x % TILE_SIZE;
        }

        int textureIdx = rays[i].wallHitContent-1;

        for (int wallPixel=wallTopPixel; wallPixel < wallBottomPixel; wallPixel++) {
            int distanceFromTop = wallPixel + (projectedWallHeight / 2) - (WINDOW_HEIGHT / 2);
            int textureOffsetY = distanceFromTop * ((float)TEXTURE_HEIGHT / projectedWallHeight);

            Color texelColor = wallTextures[textureIdx].textureBuffer[(TEXTURE_WIDTH * textureOffsetY) + textureOffsetX];

            colourBuffer[(WINDOW_WIDTH * wallPixel) + i] = texelColor;
        }

        for (int floorPixel=wallBottomPixel; floorPixel < WINDOW_HEIGHT; floorPixel++) {
            colourBuffer[(WINDOW_WIDTH * floorPixel) + i] = (Color){ .a = 0xFF, .r = 0xB3, .g = 0xB2, .b = 0xB1 };
        }
    }
}

void floodColourBuffer(Color colour) {
    for (int y=0; y < WINDOW_HEIGHT; y++) {
        for (int x=0; x < WINDOW_WIDTH; x++) {
            colourBuffer[(y * WINDOW_WIDTH) + x] = colour;
        }
    }
}

void drawToColourBuffer() {
    UpdateTexture(colourBufferTexture, colourBuffer);
    DrawTexture(colourBufferTexture, 0, 0, WHITE);
}

void drawPlayer() {
    DrawRectangle(
        gamePlayer.playerPos.x * MINIMAP_SCALE_FACTOR, 
        gamePlayer.playerPos.y * MINIMAP_SCALE_FACTOR, 
        gamePlayer.width * MINIMAP_SCALE_FACTOR, 
        gamePlayer.height * MINIMAP_SCALE_FACTOR,
        YELLOW
    );
    DrawLine(
        (gamePlayer.playerPos.x + 2.5) * MINIMAP_SCALE_FACTOR, 
        gamePlayer.playerPos.y * MINIMAP_SCALE_FACTOR,
        MINIMAP_SCALE_FACTOR * (gamePlayer.playerPos.x + 2.5 + cos(gamePlayer.rotationAngle) * 40),
        MINIMAP_SCALE_FACTOR * (gamePlayer.playerPos.y + sin(gamePlayer.rotationAngle) * 40),
        YELLOW
    );
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

void drawRays() {
    Vector2 rayDrawStartPos = (Vector2){
        (gamePlayer.playerPos.x + 2.5) * MINIMAP_SCALE_FACTOR,
        gamePlayer.playerPos.y * MINIMAP_SCALE_FACTOR
    };
    for (int i=0; i < NUM_RAYS; i++) {
        Vector2 currRayPos = (Vector2){
            rays[i].wallHitPos.x * MINIMAP_SCALE_FACTOR,
            rays[i].wallHitPos.y * MINIMAP_SCALE_FACTOR
        };
        DrawLineV(rayDrawStartPos, currRayPos, RED);
    }
}

void render() {
    BeginDrawing();
        ClearBackground(RAYWHITE);

        create3DProjection();

        drawToColourBuffer();
        floodColourBuffer(BLACK);

        drawMap();
        drawRays();
        drawPlayer();
    EndDrawing();
}

bool hasWallAtPos(Vector2 mapPos) {
    if (mapPos.x < 0 || mapPos.x > WINDOW_WIDTH || mapPos.y < 0 || mapPos.y > WINDOW_HEIGHT) {
        return true;
    }
    int mapGridIndexX = floor(mapPos.x / TILE_SIZE);
    int mapGridIndexY = floor(mapPos.y / TILE_SIZE);
    return map[mapGridIndexY][mapGridIndexX] != 0;
}