#include "display.h"
#include "triangle.h"
#include "vector.h"
#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>

// Variable ////////////////////////////////////////////////
static SDL_Window* window;
static SDL_Renderer* renderer;

static color_t* color_buffer;
static float* z_buffer;

static SDL_Texture* color_buffer_texture;
static int window_height;
static int window_width;

int render_method = WIREFRAME_AND_VERTEX;
int light_mode = LIGHT_ON;
int culling_mode = CULLING_ON;


// initialize display ---------------------------------------------------------
bool initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // Get display info
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    window_width = display_mode.w;
    window_height = display_mode.h;

    // Create SDL window
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
    
    // Create SDL renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    // Allocate the required memory in bytes to hold the color buffer
    color_buffer = (color_t*) malloc(sizeof(color_t) * window_width * window_height);
    z_buffer = (float*) malloc(sizeof(float) * window_width * window_height);
    
    // Creating a SDL texture that is used to display the color buffer
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    return true;
}

void destroy_window(void) {
    free(color_buffer);
    free(z_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Getter / Setter ------------------------------------------------------------
int get_window_width(void) {
    return window_width;
}

int get_window_height(void) {
    return window_height;
}

void set_render_mode(int method) {
    render_method = method;
}
int get_render_mode(void) {
    return render_method;
}

void set_current_light_mode(int mode) {
    light_mode = mode;
}
int get_current_light_mode(void) {
    return light_mode;
}

void set_culling_mode(int mode) {
    culling_mode = mode;
}
int get_culling_mode(void) {
    return culling_mode;
}

float get_z_buffer(int x, int y) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return 1.0;
    }
    return z_buffer[(window_width * y) + x];
}
void update_z_buffer(int x, int y, float z) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return;
    }
    z_buffer[(window_width * y) + x] = z;
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

void draw_pixel(int x, int y, color_t color) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return;
    }
    color_buffer[(window_width * y) + x] = color;
}

// Draw rect ------------------------------------------------------------------

void draw_rec(int x, int y, int w, int h, color_t color) {
    for (int j = y; j < window_height && j < y + h; j++) {
        for (int i = x; i < window_width && i < x + w; i++) {
            draw_pixel(i, j, color);
        }
    }
}

// Draw line ------------------------------------------------------------------

void draw_line_dda(vec2_t start, vec2_t end, color_t color) {
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

void draw_line_bresenham(int x0, int y0, int x1, int y1, color_t color) {
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


void draw_line(vec2_t start, vec2_t end, color_t color) {
    draw_line_bresenham(start.x, start.y, end.x, end.y, color);   
}

void draw_triangle(triangle_t triangle, color_t color) {
    for (int v = 0; v < 3; v++) {
        vec2_t start = { .x = triangle.points[v].data[0], .y = triangle.points[v].data[1] };
        vec2_t end = { .x = triangle.points[(v+1)%3].data[0], .y = triangle.points[(v+1)%3].data[1] };
        draw_line(start, end, color);
    }
}

// Buffer helper functions ----------------------------------------------------

void render_color_buffer(void) {
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(window_width * sizeof(color_t))
    );
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void clear_color_buffer(color_t color) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    for (int i = 0; i < window_width * window_height; i++) {
        color_buffer[i] = color;
    }
}

void clear_z_buffer(void) {
    for (int i = 0; i < window_width * window_height; i++) {
        z_buffer[i] = 1.0;
    }
}
