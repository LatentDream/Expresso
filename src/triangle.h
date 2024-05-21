#ifndef TRIANGLE_H
#define TRIANGLE_H

// for a face (triangle)
#include "vector.h"
#include <stdint.h>
typedef struct {
    int a, b, c;
    uint32_t color;
}  face_t;

typedef struct {
    vec2_t points[3];
    uint32_t color;
} triangle_t;

void draw_filled_triangle(triangle_t, uint32_t color);

#endif // !TRIANGLE_H
