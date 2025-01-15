#define TILE_SIZE 64
#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20
#define MINIMAP_SCALE_FACTOR 0.2f

#define WINDOW_WIDTH (MAP_NUM_COLS * TILE_SIZE)
#define WINDOW_HEIGHT (MAP_NUM_ROWS * TILE_SIZE)

#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64

#define TWO_PI 6.28318530

#define FOV_ANGLE (60 * (PI/180))
#define NUM_RAYS WINDOW_WIDTH