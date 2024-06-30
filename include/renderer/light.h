#ifndef LIGHT_H
#define LIGHT_H

#include "display.h"
#include "vector.h"

typedef struct {
    vec3_t direction;
} light_t;

color_t shade_color(color_t original_color, float factor);
light_t create_light(vec3_t direction);
light_t get_light(void);

#endif // !LIGHT_H
