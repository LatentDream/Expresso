#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "display.h"
#include "vector.h"

// Window Setting
uint32_t* color_buffer            = NULL;
SDL_Texture* color_buffer_texture = NULL;

// Display
int window_height      = 800;
int window_width       = 600;
SDL_Window* window     = NULL;
SDL_Renderer* renderer = NULL;

// Event Loop
bool is_running = false;

// other
#define N_POINTS (9 * 9 * 9)
#define fov_factor 485
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];
vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };

void setup(void) {
    // Allocate the required memory in bytes to hold the color buffer
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    
    // Creating a SDL texture that is used to display the color buffer
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    int point_count = 0;

    for (float x = -1; x <= 1; x += 0.25) {
        for (float y = -1; y <= 1; y += 0.25) {
            for (float z = -1; z <= 1; z += 0.25) {
                vec3_t new_point = { x, y, z};
                cube_points[point_count++] = new_point;
            }
        }
    }
}



void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                is_running = false;
            break;
    }
}

/* Function that receives a 3D vector and returns its projection */
vec2_t project(vec3_t point) {
    vec2_t projected_point = {
        .x = (fov_factor * (point.x / point.z)),
        .y = (fov_factor * (point.y / point.z))
    };
    return projected_point;
}

void update(void) {
    for (int i = 0; i < N_POINTS; i++) {
        vec3_t point = cube_points[i];
        // Move camera
        point.z -= camera_position.z;
        // project the curr. point
        projected_points[i] = project(point);
    }
}

void render(void) {
    // Color Pixel: 0x AA RR GG BB |  R   G  B   A
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    clear_color_buffer(0xFF000000);

    draw_ref();
    for (int i = 0; i < N_POINTS; i++) {
        vec2_t projected_point = projected_points[i];
        draw_rec(
            projected_point.x + window_width / 2,
            projected_point.y + window_height / 2,
            4,
            4,
            0xFFFFFF44
        );
    }
    render_color_buffer();
    // SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {

    // Create SDL window
    is_running = initialize_window();

    setup();

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}

