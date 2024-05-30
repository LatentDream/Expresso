#ifndef CAMERA_H
#define CAMERA_H

#include "matrix.h"
#include "vector.h"

typedef struct {
    vec3_t position;
    vec3_t direction;
    vec3_t forward_velocity;
    float  yaw_angle;
    float  pitch_angle;
} camera_t;

typedef enum { TARGET, FPS } camera_mode;

mat4_t mat4_look_at(vec3_t eye, vec3_t target);

void init_camera(vec3_t position, vec3_t direction);
vec3_t get_camera_position(void);
vec3_t get_camera_direction(void);
vec3_t get_camera_forward_velocity(void);

void update_camera_position(vec3_t position);
void update_camera_direction(vec3_t direction);
void update_camera_forward_velocity(vec3_t forward_velocity);

float get_camera_yaw_angle(void);
float get_camera_pitch_angle(void);

void rotate_camera_yaw(float angle);
void rotate_camera_pitch(float angle);

vec3_t get_camera_lookat_target(void);

#endif // !CAMERA_H
