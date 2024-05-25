#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

typedef struct {
    float u, v;
} tex2_t;

extern int texture_width;
extern int texture_height;

extern uint8_t REDBRIK_TEXTURE[];

extern uint32_t* mesh_texture;

#endif // !TEXTURE_H
