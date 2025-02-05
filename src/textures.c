#include "textures.h"
#include <stdio.h>

static char* textureFileNames[NUM_TEXTURES] = {
    "./resources/redbrick.png",
    "./resources/purplestone.png",
    "./resources/mossystone.png",
    "./resources/graystone.png",
    "./resources/colorstone.png",
    "./resources/bluestone.png",
    "./resources/wood.png",
    "./resources/eagle.png",
    "./resources/pikuma.png",
    "./resources/barrel.png",
    "./resources/light.png",
    "./resources/table.png",
    "./resources/guard.png",
    "./resources/armor.png",
};

upng_t* textures[NUM_TEXTURES];

void loadTextures() {
    for (int i=0; i < NUM_TEXTURES; i++) {
        upng_t* upng = upng_new_from_file(textureFileNames[i]);
        if (upng != NULL) {
            upng_decode(upng);
            if (upng_get_error(upng) == UPNG_EOK) {
                textures[i] = upng;
            } else {
                printf("Error when decoding texture %s\n", textureFileNames[i]);
            }
        } else {
            printf("Error loading texture %s\n", textureFileNames[i]);
        }
    }
}

void freeTextures() {
    for (int i=0; i < NUM_TEXTURES; i++) {
        upng_free(textures[i]);
    }
}