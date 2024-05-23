#include <stdio.h>
#include <immintrin.h>
#include "matrix.h"

mat4_t mat4_identity(void) {
    mat4_t identity = {0};
    identity.data[0] = 1.0f;
    identity.data[5] = 1.0f;
    identity.data[10] = 1.0f;
    identity.data[15] = 1.0f;
    return identity;
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
    mat4_t scaler = {0};
    scaler.data[0] = sx;
    scaler.data[5] = sy;
    scaler.data[10] = sz;
    scaler.data[15] = 1.0f;
    return scaler;
}

mat4_t mat4_make_translation(float tx, float ty, float tz) {
    mat4_t translator = mat4_identity();
    translator.data[12] = tx;
    translator.data[13] = ty;
    translator.data[14] = tz;
    return translator;
}

void mat4_print(mat4_t matrix) {
    for (int i = 0; i < 16; i++) {
        printf("%f ", matrix.data[i]);
        if ((i + 1) % 4 == 0) {
            printf("\n");
        }
    }
}

mat4_t mat_mult(mat4_t a, mat4_t b) {
    mat4_t result = {0};
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


mat4_t expimental__mat_mult_fast(mat4_t a, mat4_t b) {
    mat4_t result = {0};

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            __m128 sum = _mm_set1_ps(0.0f);
            for (int i = 0; i < 4; i++) {
                __m128 a_row = _mm_loadu_ps(&a.data[row * 4 + i]); // Load row of 'a'
                __m128 b_col = _mm_set1_ps(b.data[i * 4 + col]);   // Broadcast element of 'b' column
                sum = _mm_add_ps(sum, _mm_mul_ps(a_row, b_col));   // Multiply and add
            }
            _mm_storeu_ps(&result.data[row * 4 + col], sum);       // Store result in 'result'
        }
    }
    return result;
}

vec4_t mat4_mult_vec4(mat4_t m, vec4_t v) {
    vec4_t result = {0};
    for (int row = 0; row < 4; row++) {
        float sum = 0.0f;
        for (int i = 0; i < 4; i++) {
            sum += m.data[row * 4 + i] * v.data[i];
        }
        result.data[row] = sum;
    }
    return result;
}
