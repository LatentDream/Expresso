#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"
typedef struct {
    float data[16];
} mat4_t;

// Creation
mat4_t mat4_identity(void);
mat4_t mat4_make_scale(float sx, float sy, float sz);
mat4_t mat4_make_translation(float tx, float ty, float tz);
mat4_t mat4_make_rotation_z(float angle);
mat4_t mat4_make_rotation_x(float angle);
mat4_t mat4_make_rotation_y(float angle);

// Utils
void   mat4_print(mat4_t matrix);

// Ops
mat4_t mat4_mult(mat4_t a, mat4_t b);
mat4_t expimental__mat4_mult_fast(mat4_t a, mat4_t b);
vec4_t mat4_mult_vec4(mat4_t m, vec4_t v);
vec4_t expimental__mat4_mult_vec4_fast(mat4_t m, vec4_t v);

#endif // !MATRIX_H
