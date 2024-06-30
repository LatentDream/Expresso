#include "triangle.h"
#include "display.h"
#include "light.h"
#include "texture.h"
#include "upng.h"
#include "vector.h"
#include <stdint.h>
#include <stdlib.h>

vec3_t get_triangle_normal(vec4_t vertices[3]) {
    // Utils for Back culling and light shading       /*     A     */
    vec3_t vector_a = vec3_from_vec4(vertices[0]);    /*    / \    */
    vec3_t vector_b = vec3_from_vec4(vertices[1]);    /*   /   \   */
    vec3_t vector_c = vec3_from_vec4(vertices[2]);    /*  C --- B  */

    vec3_t vector_ab = vec3_sub(vector_b, vector_a);
    vec3_normalize(&vector_ab);
    vec3_t vector_ac = vec3_sub(vector_c, vector_a);
    vec3_normalize(&vector_ac);

    // Compute the face normal (using cross product to find perpendicular);
    vec3_t normal = vec3_cross(vector_ab, vector_ac);
    // Normalize the vec
    vec3_normalize(&normal);

    return normal;
}

// Helper function ============================================================
void point_swap(vec4_t* point_a, vec4_t* point_b) {
    vec4_t tmp = *point_a;
    *point_a = *point_b;
    *point_b = tmp;
}

void tex2_swap(tex2_t* point_a, tex2_t* point_b) {
    tex2_t tmp = *point_a;
    *point_a = *point_b;
    *point_b = tmp;
}

void int_swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void order_triangle_by_y(triangle_t* triangle) {
    if (triangle->points[0].data[1] > triangle->points[1].data[1]) {
        point_swap(&triangle->points[0], &triangle->points[1]);
        tex2_swap(&triangle->tex_coords[0], &triangle->tex_coords[1]);
    }
    if (triangle->points[1].data[1] > triangle->points[2].data[1]) {
        point_swap(&triangle->points[1], &triangle->points[2]);
        tex2_swap(&triangle->tex_coords[1], &triangle->tex_coords[2]);
    }
    if (triangle->points[0].data[1] > triangle->points[1].data[1]) {
        point_swap(&triangle->points[0], &triangle->points[1]);
        tex2_swap(&triangle->tex_coords[0], &triangle->tex_coords[1]);
    }
}

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {
    vec2_t ac = vec2_sub(c, a);
    vec2_t ab = vec2_sub(b, a);
    vec2_t ap = vec2_sub(p, a);
    vec2_t pc = vec2_sub(c, p);
    vec2_t pb = vec2_sub(b, p);

    float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x); // || AC x AB ||
    if (area_parallelogram_abc == 0) {
        return (vec3_t){0, 0, 0};
    }

    // Barycentric weights
    float alpha = (pc.x * pb.y - pc.y * pb.x) / area_parallelogram_abc;
    float beta = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;
    float gamma = 1 - alpha - beta;

    vec3_t weights = { alpha, beta, gamma };
    return weights;
}

// Draw =======================================================================

void draw_triangle_pixel(int x, int y, vec4_t point_a, vec4_t point_b, vec4_t point_c, color_t* color, vec3_t inverse_w, float light_intensity) {
    vec2_t point_p = { x, y };
    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    vec3_t weights = barycentric_weights(a, b, c, point_p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    // Perform the interpolation of the reciprocal w to find the depth value
    float interpolated_reciprocal_w = inverse_w.x * alpha + inverse_w.y * beta + inverse_w.z * gamma;
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    if (interpolated_reciprocal_w < get_z_buffer(x, y)) {
        // Draw the pixel with the color from the texture
        draw_pixel(x, y, shade_color(*color, light_intensity));
        // Update the z_buffer for the current pixel
        update_z_buffer(x, y, interpolated_reciprocal_w);
    }
}

void draw_texel(int x, int y, vec4_t point_a, vec4_t point_b, vec4_t point_c, tex2_t a_uv_w, tex2_t b_uv_w, tex2_t c_uv_w, upng_t* texture, vec3_t inverse_w, float light_intensity) {
    vec2_t point_p = { x, y };
    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    vec3_t weights = barycentric_weights(a, b, c, point_p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    float interpolated_u;
    float interpolated_v;
    float interpolated_reciprocal_w;

    // Perform the interpolation of all U/w V/w, 1/w values using barycentric weights
    interpolated_u = (a_uv_w.u) * alpha + (b_uv_w.u) * beta + (c_uv_w.u) * gamma;
    interpolated_v = (a_uv_w.v) * alpha + (b_uv_w.v) * beta + (c_uv_w.v) * gamma;

    // Perform the interpolation of the reciprocal w
    interpolated_reciprocal_w = inverse_w.x * alpha + inverse_w.y * beta + inverse_w.z * gamma;

    // Divide the interpolated U and V by the interpolated reciprocal w
    interpolated_u /= interpolated_reciprocal_w;
    interpolated_v /= interpolated_reciprocal_w;

    // Map the UV coordinate to the full texture width and height + clipping if error
    int texture_width = upng_get_width(texture);
    int texture_height = upng_get_height(texture);
    int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
    int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;
    
    // Adjust 1/w value
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    int position_texture = (tex_y * texture_width) + tex_x;
    if (interpolated_reciprocal_w < get_z_buffer(x, y)) {
        // Draw the pixel with the color from the texture
        uint32_t* texture_buffer = (uint32_t*)upng_get_buffer(texture);
        draw_pixel(x, y, shade_color(texture_buffer[position_texture], light_intensity));
        // Update the z_buffer for the current pixel
        update_z_buffer(x, y, interpolated_reciprocal_w);
    }
}

// Exposed function ==========================================================

void draw_filled_triangle(triangle_t triangle, color_t color) {
    // Order the triangle
    order_triangle_by_y(&triangle);
    // Extract the coordinates so its easier to work with
    int x0 = triangle.points[0].data[0];
    int y0 = triangle.points[0].data[1];
    int x1 = triangle.points[1].data[0];
    int y1 = triangle.points[1].data[1];
    int x2 = triangle.points[2].data[0];
    int y2 = triangle.points[2].data[1];

    // Vector points
    vec4_t point_a = triangle.points[0];
    vec4_t point_b = triangle.points[1];
    vec4_t point_c = triangle.points[2];
    vec3_t inverse_w = { .x = 1 / point_a.data[3], .y = 1 / point_b.data[3], .z = 1 / point_c.data[3] };

    // Render the upper part of the triangle
    // =====================================
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;
    if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);
    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_start > x_end) {
                int_swap(&x_start, &x_end);
            }

            for (int x = x_start; x <= x_end; x++) {
                draw_triangle_pixel(
                    x, y,
                    point_a,
                    point_b,
                    point_c,
                    &color,
                    inverse_w,
                    triangle.light_intensity
                );
            }
        }
    }

    // Render the lower part of the triangle
    // =====================================
    inv_slope_1 = 0;
    inv_slope_2 = 0;

    if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++) {
                draw_triangle_pixel(
                    x, y,
                    point_a,
                    point_b,
                    point_c,
                    &color,
                    inverse_w,
                    triangle.light_intensity
                );
            }
        }
    }
}

// Draw a triangle with texture
void draw_textured_triangle(triangle_t triangle) {

    order_triangle_by_y(&triangle);
    // Extract the coordinates so its easier to work with
    int x0 = triangle.points[0].data[0];
    int y0 = triangle.points[0].data[1];
    int x1 = triangle.points[1].data[0];
    int y1 = triangle.points[1].data[1];
    int x2 = triangle.points[2].data[0];
    int y2 = triangle.points[2].data[1];
    tex2_t a_uv = triangle.tex_coords[0];
    tex2_t b_uv = triangle.tex_coords[1];
    tex2_t c_uv = triangle.tex_coords[2];

    // Vector points
    vec4_t point_a = triangle.points[0];
    vec4_t point_b = triangle.points[1];
    vec4_t point_c = triangle.points[2];

    tex2_t a_uv_w = { .u = a_uv.u / point_a.data[3], .v = a_uv.v / point_a.data[3] };
    tex2_t b_uv_w = { .u = b_uv.u / point_b.data[3], .v = b_uv.v / point_b.data[3] };
    tex2_t c_uv_w = { .u = c_uv.u / point_c.data[3], .v = c_uv.v / point_c.data[3] };
    vec3_t inverse_w = { .x = 1 / point_a.data[3], .y = 1 / point_b.data[3], .z = 1 / point_c.data[3] };

    // Render the upper part of the triangle
    // =====================================
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;
    if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);
    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_start > x_end) {
                int_swap(&x_start, &x_end);
            }

            for (int x = x_start; x <= x_end; x++) {
                draw_texel(x, y, point_a, point_b, point_c, a_uv_w, b_uv_w, c_uv_w, triangle.texture, inverse_w, triangle.light_intensity);
            }
        }
    }

    // Render the lower part of the triangle
    // =====================================
    inv_slope_1 = 0;
    inv_slope_2 = 0;

    if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++) {
                // Draw our pixel with the color that comes from the texture
                draw_texel(x, y, point_a, point_b, point_c, a_uv_w, b_uv_w, c_uv_w, triangle.texture, inverse_w, triangle.light_intensity);
            }
        }
    }
}
