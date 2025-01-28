#include "player.h"
#include "map.h"
#include "ray.h"

Player player = {
    .playerPos.x = WINDOW_WIDTH / 2,
    .playerPos.y = WINDOW_HEIGHT / 2,
    .width = PLAYER_SIZE,
    .height = PLAYER_SIZE,
    .turnDirection = 0,
    .walkDirection = 0,
    .rotationAngle = PI / 2,
    .walkSpeed = PLAYER_WALK_SPEED,
    .turnSpeed = 45 * (PI / 180)
};

void handlePlayerMoveInput() {
    if (IsKeyUp(KEY_UP)) {
        player.walkDirection = 0;
    }
    if (IsKeyUp(KEY_DOWN)) {
        player.walkDirection = 0;
    }
    if (IsKeyUp(KEY_LEFT)) {
        player.turnDirection = 0;
    }
    if (IsKeyUp(KEY_RIGHT)) {
        player.turnDirection = 0;
    }
    if (IsKeyDown(KEY_UP)) {
        player.walkDirection = +1;
    }
    if (IsKeyDown(KEY_DOWN)) {
        player.walkDirection = -1;
    }
    if (IsKeyDown(KEY_LEFT)) {
        player.turnDirection = -1;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        player.turnDirection = +1;
    }
}

void movePlayer(float deltaTime) {
    player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime;
    normalizeAngle(&player.rotationAngle);
    float moveStep = player.walkDirection * player.walkSpeed * deltaTime;
    
    float newPlayerX = player.playerPos.x + cos(player.rotationAngle) * moveStep;
    float newPlayerY = player.playerPos.y + sin(player.rotationAngle) * moveStep;

    if (!wouldCollide(newPlayerX, newPlayerY)) {
        player.playerPos.x = newPlayerX;
        player.playerPos.y = newPlayerY;
    }
}

void drawPlayer() {
    DrawRectangle(
        player.playerPos.x * MINIMAP_SCALE_FACTOR, 
        player.playerPos.y * MINIMAP_SCALE_FACTOR, 
        player.width * MINIMAP_SCALE_FACTOR, 
        player.height * MINIMAP_SCALE_FACTOR,
        YELLOW
    );
    DrawLine(
        (player.playerPos.x + 2.5) * MINIMAP_SCALE_FACTOR, 
        player.playerPos.y * MINIMAP_SCALE_FACTOR,
        MINIMAP_SCALE_FACTOR * (player.playerPos.x + 2.5 + cos(player.rotationAngle) * 40),
        MINIMAP_SCALE_FACTOR * (player.playerPos.y + sin(player.rotationAngle) * 40),
        YELLOW
    );
}