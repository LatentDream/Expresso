#ifndef DISPLAY_H
#define DISPLAY_H

#include "triangle.h"
#include "vector.h"
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000.0 / FPS)

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
void draw_line(vec2_t start, vec2_t end, uint32_t color);
void draw_triangle(triangle_t triangle, uint32_t color);
void clear_color_buffer(uint32_t color);
void render_color_buffer(void);
void destroy_window(void);

#endif // DISPLAY_H
