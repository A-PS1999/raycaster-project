#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <math.h>

#define TILE_SIZE 64
#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20
#define MINIMAP_SCALE_FACTOR 0.2f

#define WINDOW_WIDTH 1707
#define WINDOW_HEIGHT 1067

#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64
#define NUM_TEXTURES 9

#define TWO_PI 6.28318530

#define FOV_ANGLE (60 * (PI/180))
#define NUM_RAYS WINDOW_WIDTH

#define PROJ_PLANE_DIST (WINDOW_WIDTH/2) / tan(FOV_ANGLE/2)

#endif