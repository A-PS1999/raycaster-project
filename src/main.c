#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "raylib.h"
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
void drawMap();
void drawRays();
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
    gamePlayer.playerPos.x = WINDOW_WIDTH / 2;
    gamePlayer.playerPos.y = WINDOW_HEIGHT / 2;
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
    castRays();
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
    float rayAngle = gamePlayer.rotationAngle - (FOV_ANGLE / 2);

    for (int stripId = 0; stripId < NUM_RAYS; stripId++) {
        castRay(rayAngle, stripId);

        rayAngle += FOV_ANGLE / NUM_RAYS;
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

    float horizontalHitDistance = foundHorizontalWall ? Vector2Distance(gamePlayer.playerPos, (Vector2){horizontalWallHitX, horizontalWallHitY}) : INT_MAX;
    float verticalHitDistance = foundVerticalWall ? Vector2Distance(gamePlayer.playerPos, (Vector2){verticalWallHitX, verticalWallHitY}) : INT_MAX;

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

void drawPlayer() {
    DrawRectangle(
        gamePlayer.playerPos.x * MINIMAP_SCALE_FACTOR, 
        gamePlayer.playerPos.y * MINIMAP_SCALE_FACTOR, 
        gamePlayer.width * MINIMAP_SCALE_FACTOR, 
        gamePlayer.height * MINIMAP_SCALE_FACTOR,
        YELLOW
    );
    DrawLine(
        gamePlayer.playerPos.x + 2.5 * MINIMAP_SCALE_FACTOR, 
        gamePlayer.playerPos.y * MINIMAP_SCALE_FACTOR,
        gamePlayer.playerPos.x + 2.5 + cos(gamePlayer.rotationAngle) * 40,
        gamePlayer.playerPos.y + sin(gamePlayer.rotationAngle) * 40,
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
        gamePlayer.playerPos.x + 2.5 * MINIMAP_SCALE_FACTOR,
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