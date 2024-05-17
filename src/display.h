#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL_video.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

// Varaible ////////////////////////////////////////////////
extern uint32_t* color_buffer;
extern SDL_Texture* color_buffer_texture;
extern int window_height;
extern int window_width;
extern SDL_Window* window;
extern SDL_Renderer* renderer;

// Function ////////////////////////////////////////////////
bool initialize_window(void);
void draw_grid(void);
void draw_ref(void);
void draw_pixel(int x, int y, uint32_t color);
void draw_rec(int x, int y, int w, int h, uint32_t color);
void clear_color_buffer(uint32_t color);
void render_color_buffer(void);
void destroy_window(void);

#endif // DISPLAY_H
