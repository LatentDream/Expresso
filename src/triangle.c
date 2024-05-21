#include "triangle.h"
#include "display.h"
#include "vector.h"

void point_swap(vec2_t* point_a, vec2_t* point_b) {
    vec2_t tmp = *point_a;
    *point_a = *point_b;
    *point_b = tmp;
}

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

void draw_filled_triangle(triangle_t triangle, color_t color) {
    // Order the triangle
    if (triangle.points[0].y > triangle.points[1].y) {
        point_swap(&triangle.points[0], &triangle.points[1]);
    }
    if (triangle.points[1].y > triangle.points[2].y) {
        point_swap(&triangle.points[1], &triangle.points[2]);
    }
    if (triangle.points[0].y > triangle.points[1].y) {
        point_swap(&triangle.points[0], &triangle.points[1]);
    }

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

