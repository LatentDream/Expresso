#include "display.h"
#include "triangle.h"
#include "vector.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>

bool initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // Get display info
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    window_width = display_mode.w / 2;
    window_height = display_mode.h / 1.5;

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}

// Draw grid ------------------------------------------------------------------

void draw_grid(void) {
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            if (y % 20 == 0 || x % 20 == 0) {
                draw_pixel(x, y, 0xFF333333);
            }
        }
    }
}

void draw_ref(void) {
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            if (y % 20 == 0 && x % 20 == 0) {
                draw_pixel(x, y, 0xFF333333);
            }
        }
    }
}

// Draw pixel -----------------------------------------------------------------

void draw_pixel(int x, int y, uint32_t color) {
    if (x < window_width && x >= 0 && y < window_height && y >= 0) {
        color_buffer[(window_width * y) + x] = color;
    }
}

// Draw rect ------------------------------------------------------------------

void draw_rec(int x, int y, int w, int h, uint32_t color) {
    for (int j = y; j < window_height && j < y + h; j++) {
        for (int i = x; i < window_width && i < x + w; i++) {
            draw_pixel(i, j, color);
        }
    }
}

// Draw line ------------------------------------------------------------------

void draw_line_dda(vec2_t start, vec2_t end, uint32_t color) {
    int delta_x = (end.x - start.x);
    int delta_y = (end.y - start.y);

    int side_len = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

    float x_inc = delta_x / (float)side_len;
    float y_inc = delta_y / (float)side_len;

    float current_x = start.x;
    float current_y = start.y;

    for (int i = 0; i < side_len; i++) {
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void draw_line_bresenham(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;

    while (true) {
        draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2*error;
        if (e2 > dy) {
            if (x0 == x1) break;
            error += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            if (y0 == y1) break;
            error += dx;
            y0 += sy;
        }
    }
}


void draw_line(vec2_t start, vec2_t end, uint32_t color) {
    draw_line_bresenham(start.x, start.y, end.x, end.y, color);   
}

void draw_triangle(triangle_t triangle, uint32_t color) {
    for (int v = 0; v < 3; v++) {
        draw_line(triangle.points[(v)%3], triangle.points[(v+1)%3], color);
    }
}

// Buffer helper functions ----------------------------------------------------

void render_color_buffer(void) {
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(window_width * sizeof(uint32_t))
    );
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color) {
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            color_buffer[(window_width * y) + x] = color;
        }
    }
}

void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
