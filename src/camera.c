#include "matrix.h"
#include "vector.h"
#include "camera.h"

const vec3_t CAMERA_UP = {0, 1, 0};

static camera_t camera = {
    .position = { .x = 0, .y = 0, .z = 0 },
    .direction = { .x = 0, .y = 0, .z = 1 },
    .forward_velocity = { .x = 0, .y = 0, .z = 0 },
    .yaw_angle = 0.0,
    .pitch_angle = 0.0
};



void init_camera(vec3_t position, vec3_t direction) {
    camera.position = position;
    camera.direction = direction;
}
vec3_t get_camera_position(void) {
    return camera.position;
}
vec3_t get_camera_direction(void) {
    return camera.direction;
}
vec3_t get_camera_forward_velocity(void) {
    return camera.forward_velocity;
}

void update_camera_position(vec3_t position) {
    camera.position = position;
}
void update_camera_direction(vec3_t direction) {
    camera.direction = direction;
}
void update_camera_forward_velocity(vec3_t forward_velocity) {
    camera.forward_velocity = forward_velocity;
}

float get_camera_yaw_angle(void) {
    return camera.yaw_angle;
}
float get_camera_pitch_angle(void) {
    return camera.pitch_angle;
}

void rotate_camera_yaw(float angle) {
    camera.yaw_angle += angle;
}

void rotate_camera_pitch(float angle) {
    camera.pitch_angle += angle;
}

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

vec3_t get_camera_lookat_target(void) {
    // Initialize the target looking at the positive z-axis
    vec3_t target = { 0, 0, 1 };
    
    mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw_angle);
    mat4_t camera_pitch_rotation = mat4_make_rotation_x(camera.pitch_angle);

    // Create camera rotation matrix based on yaw and pitch
    mat4_t camera_rotation = mat4_identity();
    camera_rotation = mat4_mult(camera_pitch_rotation, camera_rotation);
    camera_rotation = mat4_mult(camera_yaw_rotation, camera_rotation);

    // Update camera direction based on the rotation
    vec4_t camera_direction = mat4_mult_vec4(camera_rotation, vec4_from_vec3(target));
    camera.direction = vec3_from_vec4(camera_direction);

    // Offset the camera position in the direction where the camera is pointing at
    target = vec3_add(camera.position, camera.direction);

    return target;
}

