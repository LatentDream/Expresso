#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    float x;
    float y;
} vec2_t;

typedef struct {
    float x;
    float y;
    float z;
} vec3_t;

typedef struct {
    vec3_t position;
    vec3_t rotation;
    float fov_angle;
} camera_t;

#endif // !VECTOR_H
