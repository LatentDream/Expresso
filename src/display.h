#ifndef DISPLAY_H
#define DISPLAY_H

#include "triangle.h"
#include "vector.h"
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000.0 / FPS)

typedef enum { WIREFRAME,
    WIREFRAME_AND_VERTEX,
    TRIANGLE,
    TRIANGLE_AND_WIREFRAME, 
    TEXTURE,
    TEXTURE_AND_WIREFRAME,
} rendering_mode;
typedef enum { CULLING_ON, CULLING_OFF } culling_mode;
typedef enum { LIGHT_ON, LIGHT_OFF } light_mode;
typedef uint32_t color_t;

// Variable ////////////////////////////////////////////////
extern color_t* color_buffer;
extern SDL_Texture* color_buffer_texture;
extern int window_height;
extern int window_width;
extern SDL_Window* window;
extern SDL_Renderer* renderer;

// Function ////////////////////////////////////////////////
bool initialize_window(void);
void draw_grid(void);
void draw_ref(void);
void draw_pixel(int x, int y, color_t color);
void draw_rec(int x, int y, int w, int h, color_t color);
void draw_line(vec2_t start, vec2_t end, color_t color);
void draw_triangle(triangle_t triangle, color_t color);
void clear_color_buffer(color_t color);
void render_color_buffer(void);
void destroy_window(void);

#endif // DISPLAY_H
