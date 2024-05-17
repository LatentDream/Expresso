#include "display.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

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

void draw_pixel(int x, int y, uint32_t color) {
    if (x < window_width && x >= 0 && y < window_height && y >= 0) {
        color_buffer[(window_width * y) + x] = color;
    }
}

void draw_rec(int x, int y, int w, int h, uint32_t color) {
    for (int j = y; j < window_height && j < y + h; j++) {
        for (int i = x; i < window_width && i < x + w; i++) {
            draw_pixel(i, j, color);
        }
    }
}

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
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
