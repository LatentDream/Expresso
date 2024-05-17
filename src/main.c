#include <SDL2/SDL_timer.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"

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
int previous_frame_time = 0;

// other -- Meshes
#define fov_factor 485
vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };
vec3_t cube_rotation = { .x = 0, .y = 0, .z = 0 };
triangle_t triangle_to_render[N_MESH_FACES];

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

/* Function that receives a 3D vector and returns its projection */
vec2_t project(vec3_t point) {
    vec2_t projected_point = {
        .x = (fov_factor * (point.x / point.z)),
        .y = (fov_factor * (point.y / point.z))
    };
    return projected_point;
}

void update(void) {
    // Limit the tick to the FRAME_TARGET_TIME
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    while (!SDL_TICKS_PASSED(
        SDL_GetTicks(),
        previous_frame_time + FRAME_TARGET_TIME))
    {
        SDL_Delay(time_to_wait);
    }
    previous_frame_time = SDL_GetTicks();

    // Our movement
    cube_rotation.x += 0.01;
    cube_rotation.y += 0.03;
    cube_rotation.z += 0.03;

    // Loop over the triangle faces
    for (int i = 0; i < N_MESH_FACES; i++) {
        face_t mesh_face = mesh_faces[i];

        vec3_t face_vertices[mesh_face.a];
        face_vertices[0] = mesh_vertices[mesh_face.a - 1];
        face_vertices[1] = mesh_vertices[mesh_face.b - 1];
        face_vertices[2] = mesh_vertices[mesh_face.c - 1];

        // Loop over the vertices and apply the transformation and save it for the renderer
        triangle_t projected_triangle;
        for (int j = 0; j < 3; j++) {
            vec3_t transformed_vertex = face_vertices[j];

            // Accumulate the postion of transform xyz
            transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z);

            // Translate away from the camera
            transformed_vertex.z -= camera_position.z;

            // Project the point in 2D
            vec2_t projected_point = project(transformed_vertex);

            // Scale and place in the middle
            projected_point.x += (window_width/2);
            projected_point.y += (window_height/2);
            
            projected_triangle.points[j] = projected_point;

        }
        // Save the projected tri. for the renderer
        triangle_to_render[i] = projected_triangle;
    }
}

void render(void) {
    // Clear buffer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    clear_color_buffer(0xFF000000);
    draw_ref();

    // Render all the triangle that need to be renderer
    uint32_t color = 0xFF565422;
    for (int i = 0; i < N_MESH_FACES; i++) {
        triangle_t triangle = triangle_to_render[i];
        draw_rec(triangle.points[0].x, triangle.points[0].y, 3, 3, color);
        draw_rec(triangle.points[1].x, triangle.points[1].y, 3, 3, color);
        draw_rec(triangle.points[2].x, triangle.points[2].y, 3, 3, color);
    }

    // Render
    render_color_buffer();
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

