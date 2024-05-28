#include "matrix.h"
#include "vector.h"
#include "camera.h"

const vec3_t CAMERA_UP = {0, 1, 0};
camera_t camera = {
    .position = { .x = 0, .y = 0, .z = 0 },
    .direction = { .x = 0, .y = 0, .z = 1 },
    .forward_velocity = { .x = 0, .y = 0, .z = 0 },
    .yaw_angle = 0.0,
};


mat4_t mat4_look_at(vec3_t eye, vec3_t target) {

    vec3_t z = vec3_sub(target, eye);
    vec3_normalize(&z);
    vec3_t x = vec3_cross(CAMERA_UP, z);
    vec3_normalize(&x);
    vec3_t y = vec3_cross(z, x);

    mat4_t view_matrix = {
        .data = {
            x.x, x.y, x.z, -vec3_dot_product(x, eye),
            y.x, y.y, y.z, -vec3_dot_product(y, eye),
            z.x, z.y, z.z, -vec3_dot_product(z, eye),
            0,   0,   0,   1
        }
    };
    
    return view_matrix;
} 
