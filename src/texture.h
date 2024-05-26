#ifndef TEXTURE_H
#define TEXTURE_H

#include "upng.h"
#include <stdint.h>

typedef struct {
    float u, v;
} tex2_t;

extern int texture_width;
extern int texture_height;
extern uint8_t REDBRICK_TEXTURE[];
extern uint32_t* mesh_texture;
extern upng_t* upng_texture;

void load_png_texture_data(char* filename);

#endif // !TEXTURE_H
