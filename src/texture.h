#ifndef TEXTURE_H
#define TEXTURE_H

#include "upng.h"
#include <stdint.h>

typedef struct {
    float u, v;
} tex2_t;

tex2_t tex2_clone(tex2_t* tex);

#endif // !TEXTURE_H
