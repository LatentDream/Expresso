#ifndef LIGHT_H
#define LIGHT_H

#include "display.h"
#include "vector.h"

typedef struct {
    vec3_t direction;
} light_t;

extern light_t light;

color_t shade_color(color_t original_color, float factor);

#endif // !LIGHT_H
