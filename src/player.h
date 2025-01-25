#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

typedef struct {
    Vector2 playerPos;
    float width;
    float height;
    int turnDirection; // -1 for left, 1 for right
    int walkDirection; // -1 for back, 1 for forwards
    float rotationAngle;
    float walkSpeed;
    float turnSpeed;
} Player;

extern Player player;

void handlePlayerMoveInput();
void drawPlayer();
void movePlayer(float deltaTime);

#endif