#ifndef DISPLAY_H
#define DISPLAY_H

#include "triangle.h"
#include "vector.h"
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define FPS 60
#define FRAME_TARGET_TIME (1000.0 / FPS)

enum rendering_mode { WIREFRAME,
    WIREFRAME_AND_VERTEX,
    TRIANGLE,
    TRIANGLE_AND_WIREFRAME, 
    TEXTURE,
    TEXTURE_AND_WIREFRAME,
};
enum culling_mode { CULLING_ON, CULLING_OFF };
enum light_mode { LIGHT_ON, LIGHT_OFF };

typedef uint32_t color_t;

// Function ////////////////////////////////////////////////
bool initialize_window(bool is_fullscreen, bool is_retro_look);
void destroy_window(void);

// Drawing ////////////////////////////////////////////////
void draw_grid(void);
void draw_ref(void);
void draw_pixel(int x, int y, color_t color);
void draw_rec(int x, int y, int w, int h, color_t color);
void draw_line(vec2_t start, vec2_t end, color_t color);
void draw_triangle(triangle_t triangle, color_t color);

// Rendering //////////////////////////////////////////////
void clear_color_buffer(color_t color);
void render_color_buffer(void);
void clear_z_buffer(void);
float get_z_buffer(int x, int y);
void update_z_buffer(int x, int y, float z);

// Getter / Setter /////////////////////////////////////////
int get_window_height(void);
int get_window_width(void);

void set_render_mode(int method);
int get_render_mode(void);
void set_current_light_mode(int light_mode);
int get_current_light_mode(void);
void set_culling_mode(int culling_mode);
int get_culling_mode(void);


#endif // DISPLAY_H
