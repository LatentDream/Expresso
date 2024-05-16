#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>

#define window_height   600
#define window_width    800

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
bool is_running = false;
uint32_t* color_buffer = NULL;

bool init_window(void) {
    // Init SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL Windows.\n");
        return false;
    }

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
        fprintf(stderr, "Error creating SDL Windows.\n");
        return false;
    }

    // Create SDL renderer
    renderer = SDL_CreateRenderer(window, -1, 0); 
    if (!renderer) {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        return false;
    }
    return true;
}

void setup(void) {
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                is_running = false;
            }
            break;
    }
}

void update(void) {}

void render(void) {
    // Color Pixel: 0x AA RR GG BB |  R   G  B   A
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {

    // Create SDL window
    bool is_running = init_window();

    setup();

    while(is_running) {
        process_input();
        update();
        render();
    }

}

