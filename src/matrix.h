#ifndef MATRIX_H
#define MATRIX_H

typedef struct {
    float data[16];
} mat4_t;

mat4_t mat4_identity(void);
mat4_t mat4_make_scale(float sx, float sy, float sz);
mat4_t mat4_make_translation(float tx, float ty, float tz);
void mat4_print(mat4_t matrix);
mat4_t mat_mult(mat4_t a, mat4_t b);
mat4_t expimental__mat_mult_fast(mat4_t a, mat4_t b);
#endif // !MATRIX_H
