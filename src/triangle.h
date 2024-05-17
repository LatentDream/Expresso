#ifndef TRIANGLE_H
#define TRIANGLE_H

// for a face (triangle)
#include "vector.h"
typedef struct {
    int a, b, c;
}  face_t;

typedef struct {
    vec2_t points[3];
} triangle_t;

#endif // !TRIANGLE_H
