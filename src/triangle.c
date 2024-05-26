#include "triangle.h"
#include "display.h"
#include "vector.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

void draw_texel(int x, int y, vec2_t point_a, vec2_t point_b, vec2_t point_c, float u0, float v0, float u1, float v1, float u2, float v2, uint32_t* texture) {
    vec2_t point_p = { x, y };
    vec3_t weights = barycentric_weights(point_a, point_b, point_c, point_p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    // Perform the interpolation of all U and V values using barycentric weights
    float interpolated_u = (u0) * alpha + (u1) * beta + (u2) * gamma;
    float interpolated_v = (v0) * alpha + (v1) * beta + (v2) * gamma;

    // Map the UV coordinate to the full texture width and height
    int tex_x = abs((int)(interpolated_u * texture_width));
    int tex_y = abs((int)(interpolated_v * texture_height));
    
    if ((tex_y * texture_width) + tex_x < texture_width * texture_height) {
        draw_pixel(x, y, texture[(texture_width * tex_y) + tex_x]);
    }

}

// Draw a triangle ===========================================================

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color) {
    // Find the two slope
    float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
    float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);
    
    // Start x_start and x_end from the top vertex (x0, y0)
    float x_start = x0;
    float x_end = x0;

    // Scanlines from top to bottom
    for (int y = y0; y <= y2; y++) {
        vec2_t start = {.x = x_start, .y = y};
        vec2_t end = {.x = x_end, .y = y};
        draw_line(start, end, color);
        x_start += inv_slope_1;
        x_end += inv_slope_2;
    }
}

void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color) {
    float inv_slope_1 = (float)(x2 - x0) / (y2 - y0);
    float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);
    // Start x_start and x_end from the top vertex (x0, y0)
    float x_start = x2;
    float x_end = x2;
    // Scanlines from bottom to top
    for (int y = y2; y >= y0; y--) {
        vec2_t start = {.x = x_start, .y = y};
        vec2_t end = {.x = x_end, .y = y};
        draw_line(start, end, color);
        x_start -= inv_slope_1;
        x_end -= inv_slope_2;
    }

}

// Exposed function ==========================================================

void draw_filled_triangle(triangle_t triangle, color_t color) {
    // Order the triangle
    order_triangle_by_y(&triangle);

    if (triangle.points[1].data[1] == triangle.points[2].data[1]) {
        // Draw the flat-bottom triangle
        fill_flat_bottom_triangle(
            triangle.points[0].data[0],
            triangle.points[0].data[1],
            triangle.points[1].data[0],
            triangle.points[1].data[1], 
            triangle.points[2].data[0],
            triangle.points[2].data[1], 
            color);
    } else if (triangle.points[0].data[1] == triangle.points[1].data[1]) {
        // Draw the flat-bottom triangle only
        fill_flat_top_triangle(
            triangle.points[0].data[0],
            triangle.points[0].data[1],
            triangle.points[1].data[0],
            triangle.points[1].data[1], 
            triangle.points[2].data[0],
            triangle.points[2].data[1], 
            color);
    } else {
        // Calculate the new vertex (Mx, My) using the triangle similarity
        int My = triangle.points[1].data[1];
        int Mx = ((float)((triangle.points[2].data[0] - triangle.points[0].data[0]) * (triangle.points[1].data[1] - triangle.points[0].data[1])) / (float)(triangle.points[2].data[1] - triangle.points[0].data[1])) + triangle.points[0].data[0];

        // Draw the flat-bottom triangle
        fill_flat_bottom_triangle(
            triangle.points[0].data[0],
            triangle.points[0].data[1],
            triangle.points[1].data[0],
            triangle.points[1].data[1],
            Mx, My, color); 
        
        // Draw the flat-top triangle
        fill_flat_top_triangle(
            triangle.points[1].data[0],
            triangle.points[1].data[1],
            Mx, My,
            triangle.points[2].data[0],
            triangle.points[2].data[1],
            color); 
    }
}

// Draw a triangle with texture
void draw_textured_triangle(triangle_t triangle, uint32_t* texture) {

    order_triangle_by_y(&triangle);
    // Extract the coordinates so its easier to work with
    int x0 = triangle.points[0].data[0];
    int y0 = triangle.points[0].data[1];
    int z0 = triangle.points[0].data[2];
    int x1 = triangle.points[1].data[0];
    int y1 = triangle.points[1].data[1];
    int z1 = triangle.points[1].data[2];
    int x2 = triangle.points[2].data[0];
    int y2 = triangle.points[2].data[1];
    int z2 = triangle.points[2].data[2];
    float u0 = triangle.tex_coords[0].u;
    float v0 = triangle.tex_coords[0].v;
    float u1 = triangle.tex_coords[1].u;
    float v1 = triangle.tex_coords[1].v;
    float u2 = triangle.tex_coords[2].u;
    float v2 = triangle.tex_coords[2].v;

    // Vector points
    vec2_t point_a = {.x = x0, .y = y0};
    vec2_t point_b = {.x = x1, .y = y1};
    vec2_t point_c = {.x = x2, .y = y2};

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
                draw_texel(x, y, point_a, point_b, point_c, u0, v0, u1, v1, u2, v2, texture);
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
                draw_texel(x, y, point_a, point_b, point_c, u0, v0, u1, v1, u2, v2, texture);
            }
        }
    }


}
