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
    float data[4];
} vec4_t;

typedef struct {
    vec3_t position;
    vec3_t rotation;
    float fov_angle;
} camera_t;

// Vector manipulation functions -------------------------
// 2D

float  vec2_length(vec2_t v);
vec2_t vec2_add(vec2_t a, vec2_t b);
vec2_t vec2_sub(vec2_t a, vec2_t b);
vec2_t vec2_mult(vec2_t a, float factor);
float  vec2_doc_product(vec2_t a, vec2_t b);
void   vec2_normalize(vec2_t* a);

// 3D
vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);
float  vec3_length(vec3_t v);
vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_sub(vec3_t a, vec3_t b);
vec3_t vec3_mult(vec3_t a, float factor);
vec3_t vec3_cross(vec3_t a, vec3_t b);
float  vec3_doc_product(vec3_t a, vec3_t b);
void   vec3_normalize(vec3_t* a);

// 4D

// Convertions -------------------------------------------
vec4_t vec4_from_vec3(vec3_t v);
vec3_t vec3_from_vec4(vec4_t v);

#endif // !VECTOR_H s
