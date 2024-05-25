#ifndef TRIANGLE_H
#define TRIANGLE_H

// for a face (triangle)
#include "texture.h"
#include "vector.h"
#include <stdint.h>
typedef struct {
    int a, b, c;
    tex2_t a_uv, b_uv, c_uv;
    uint32_t color;
}  face_t;

typedef struct {
    vec2_t points[3];
    tex2_t tex_coords[3];
    uint32_t color;
    float avg_depth;     // Expensibe ? TODO
} triangle_t;

void draw_filled_triangle(triangle_t triangle, uint32_t color);
void draw_textured_triangle(triangle_t triangle, uint32_t* texture);

#endif // !TRIANGLE_H
