#ifndef CAMERA_H
#define CAMERA_H

#include "matrix.h"

typedef struct {
    vec3_t position;
    vec3_t direction;
} camera_t;

extern camera_t camera;

mat4_t mat4_look_at(vec3_t eye, vec3_t target);

#endif // !CAMERA_H
