#include <math.h>
#include <stdio.h>
#include <immintrin.h>
#include "matrix.h"
#include "vector.h"
#include <omp.h>

// Create Matrix Functions ======================================

mat4_t mat4_identity(void) {
    mat4_t identity = {0};
    identity.data[0] = 1.0f;
    identity.data[5] = 1.0f;
    identity.data[10] = 1.0f;
    identity.data[15] = 1.0f;
    return identity;
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
    mat4_t scaler = mat4_identity();
    scaler.data[0] = sx;
    scaler.data[5] = sy;
    scaler.data[10] = sz;
    scaler.data[15] = 1.0f;
    return scaler;
}

mat4_t mat4_make_translation(float tx, float ty, float tz) {
    mat4_t translator = mat4_identity();
    translator.data[3]  = tx;
    translator.data[7]  = ty;
    translator.data[11] = tz;
    return translator;
}

mat4_t mat4_make_rotation_z(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    mat4_t result = mat4_identity();
    result.data[0] = c;
    result.data[1] = s;
    result.data[4] = -s;
    result.data[5] = c;
    return result;
}

mat4_t mat4_make_rotation_x(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    mat4_t result = mat4_identity();
    result.data[5]  = c;
    result.data[6]  = -s;
    result.data[9]  = s;
    result.data[10] = c;
    return result;
}

mat4_t mat4_make_rotation_y(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    mat4_t result = mat4_identity();
    result.data[0]  = c;
    result.data[2]  = s;
    result.data[8]  = -s;
    result.data[10] = c;
    return result;
}

mat4_t mat4_make_perspective(float fov, float aspect, float z_near, float z_far) {
    if (z_near == z_far || aspect == 0.0f) {
        printf("[ERROR] Invalid perspective matrix parameters\n");
        return mat4_identity();
    }
    mat4_t result = {0};
    float tan_half_fov = tan(fov / 2.0f);
    result.data[0]  = aspect * (1.0f / (tan_half_fov));
    result.data[5]  = 1.0f / tan_half_fov;
    result.data[10] = z_far / (z_far - z_near);
    result.data[11] = -(z_far * z_near) / (z_far - z_near);
    result.data[14] = 1.0f;  // To save the original z value (for perspective division, etc.)
    return result;
}

vec4_t mat4_mul_vec4_project(mat4_t mat_proj, vec4_t v) {
    // Multiply the vector by the projection matrix
    vec4_t result = mat4_mult_vec4(mat_proj, v);
    if (result.data[3] != 0.0f) {
        // Nomalize the vector between -1 and 1
        result.data[0] /= result.data[3];
        result.data[1] /= result.data[3];
        result.data[2] /= result.data[3];
    }
    return result;
}

// Debugging Functions ==========================================

void mat4_print(mat4_t matrix) {
    for (int i = 0; i < 16; i++) {
        printf("%f ", matrix.data[i]);
        if ((i + 1) % 4 == 0) {
            printf("\n");
        }
    }
}

// Matrix Math Functions ========================================

mat4_t mat4_mult(mat4_t a, mat4_t b) {
    mat4_t result = {0};
    #pragma omp parallel for
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            float sum = 0.0f;
            for (int i = 0; i < 4; i++) {
                sum += a.data[row * 4 + i] * b.data[i * 4 + col];
            }
            result.data[row * 4 + col] = sum;
        }
    }
    return result;
}

// Matrix Vector Functions ======================================

vec4_t mat4_mult_vec4(mat4_t m, vec4_t v) {
    vec4_t result = {0};
    #pragma omp parallel for
    for (int row = 0; row < 4; row++) {
        float sum = 0.0f;
        for (int i = 0; i < 4; i++) {
            sum += m.data[row * 4 + i] * v.data[i];
        }
        result.data[row] = sum;
    }
    return result;
}
