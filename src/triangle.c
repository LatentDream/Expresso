#include "triangle.h"
#include "display.h"
#include "vector.h"
#include <stdint.h>
#include <stdlib.h>

// Helper function ============================================================
void point_swap(vec2_t* point_a, vec2_t* point_b) {
    vec2_t tmp = *point_a;
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
    if (triangle->points[0].y > triangle->points[1].y) {
        point_swap(&triangle->points[0], &triangle->points[1]);
        tex2_swap(&triangle->tex_coords[0], &triangle->tex_coords[1]);
    }
    if (triangle->points[1].y > triangle->points[2].y) {
        point_swap(&triangle->points[1], &triangle->points[2]);
        tex2_swap(&triangle->tex_coords[1], &triangle->tex_coords[2]);
    }
    if (triangle->points[0].y > triangle->points[1].y) {
        point_swap(&triangle->points[0], &triangle->points[1]);
        tex2_swap(&triangle->tex_coords[0], &triangle->tex_coords[1]);
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

    if (triangle.points[1].y == triangle.points[2].y) {
        // Draw the flat-bottom triangle
        fill_flat_bottom_triangle(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x, triangle.points[1].y, triangle.points[2].x, triangle.points[2].y, color);
    } else if (triangle.points[0].y == triangle.points[1].y) {
        // Draw the flat-bottom triangle only
        fill_flat_top_triangle(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x, triangle.points[1].y, triangle.points[2].x, triangle.points[2].y, color);
    } else {
        // Calculate the new vertex (Mx, My) using the triangle similarity
        int My = triangle.points[1].y;
        int Mx = ((float)((triangle.points[2].x - triangle.points[0].x) * (triangle.points[1].y - triangle.points[0].y)) / (float)(triangle.points[2].y - triangle.points[0].y)) + triangle.points[0].x;

        // Draw the flat-bottom triangle
        fill_flat_bottom_triangle(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x, triangle.points[1].y, Mx, My, color); 
        
        // Draw the flat-top triangle
        fill_flat_top_triangle(triangle.points[1].x, triangle.points[1].y, Mx, My, triangle.points[2].x, triangle.points[2].y, color); 
    }
}



// Draw a triangle with texture
void draw_textured_triangle(triangle_t triangle, uint32_t* texture) {
    order_triangle_by_y(&triangle);
    // Extract the coordinates so its easier to work with
    int x0 = triangle.points[0].x;
    int y0 = triangle.points[0].y;
    int x1 = triangle.points[1].x;
    int y1 = triangle.points[1].y;
    int x2 = triangle.points[2].x;
    int y2 = triangle.points[2].y;
    float u0 = triangle.tex_coords[0].u;
    float v0 = triangle.tex_coords[0].v;
    float u1 = triangle.tex_coords[1].u;
    float v1 = triangle.tex_coords[1].v;
    float u2 = triangle.tex_coords[2].u;
    float v2 = triangle.tex_coords[2].v;

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
                // todo: interpolate the u and v + texture look up
                draw_pixel(x, y, (x%2 == 0 && y%2 == 0) ? 0xFF8A50FF:0xFF000000);
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

            if (x_start > x_end) {
                int_swap(&x_start, &x_end);
            }

            for (int x = x_start; x <= x_end; x++) {
                // todo: interpolate the u and v + texture look up
                draw_pixel(x, y, (x%2 == 0 && y%2 == 0) ? 0xFF8A50FF:0xFF000000);
            }
        }
    }


}
