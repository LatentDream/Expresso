#ifndef CAMERA_H
#define CAMERA_H

#include "matrix.h"

typedef struct {
    vec3_t position;
    vec3_t direction;
    vec3_t forward_velocity;
    float  yaw_angle;
} camera_t;

typedef enum { TARGET, FPS } camera_mode;

extern camera_t camera;

mat4_t mat4_look_at(vec3_t eye, vec3_t target);

#endif // !CAMERA_H
