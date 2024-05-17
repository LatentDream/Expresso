#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "display.h"

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

void update(void) {
    // todo
}


void render(void) {
    // Color Pixel: 0x AA RR GG BB |  R   G  B   A
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    render_color_buffer();
    // clear_color_buffer(0xFFFFFFFF);
    draw_ref();
    draw_rec(200, 400, 250, 250, 0xFF000000);

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

