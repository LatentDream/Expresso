#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "array.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "texture.h"
#include "upng.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"

// Window Setting
color_t* color_buffer            = NULL;
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
triangle_t* triangle_to_render = NULL;
culling_mode current_culling_mode = CULLING_ON;
rendering_mode current_rendering_mode = TRIANGLE_AND_WIREFRAME;
color_t COLOR_CONTRAST = 0xFF1154BB;
light_mode current_light_mode = LIGHT_ON;
mat4_t perspective;


// Setup Function ==============================================================

void setup(void) {
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

    // Init the perspective matrix
    float fov = 1.047194;  // 60 degrees in radians
    float aspect = (float)window_height / (float)window_width;
    float near = 0.1;
    float far = 100.0;
    perspective = mat4_make_perspective(fov, aspect, near, far);

    // // CUBE EXMAPLE
    // // Hardcoded cube
    // Manually load the texture + convert to 32bits
    // mesh_texture = (uint32_t*)REDBRICK_TEXTURE;
    // texture_width = 64;
    // texture_height = 64;
    // load_cube_example_mesh();
    // load_png_texture_data("./assets/cube.png");

    // Real 3D models
    // load_mesh_from_obj_simple("./assets/teapot.obj", 0xFFFF5400);
    // load_mesh_and_texture_from_obj("./assets/cube.obj", 0xFFFF5400);
    load_mesh_and_texture_from_obj("./assets/f22.obj", 0xFFFF5400);
    load_png_texture_data("./assets/f22.png");
}

void free_ressources(void) {
    free(color_buffer);
    free(z_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
    upng_free(upng_texture);
}

// Process Input Function ======================================================

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
            if (event.key.keysym.sym == SDLK_c)
                current_culling_mode = (current_culling_mode + 1) % 2;
            if (event.key.keysym.sym == SDLK_y)
                current_rendering_mode = WIREFRAME_AND_VERTEX;
            if (event.key.keysym.sym == SDLK_u)
                current_rendering_mode = WIREFRAME;
            if (event.key.keysym.sym == SDLK_i)
                current_rendering_mode = TRIANGLE;
            if (event.key.keysym.sym == SDLK_o)
                current_rendering_mode = TRIANGLE_AND_WIREFRAME;
            if (event.key.keysym.sym == SDLK_t)
                current_rendering_mode = TEXTURE;
            if (event.key.keysym.sym == SDLK_g)
                current_rendering_mode = TEXTURE_AND_WIREFRAME;
            if (event.key.keysym.sym == SDLK_l)
                current_light_mode = (current_light_mode + 1) % 2;
            break;
    }
}

// Update Function =============================================================

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

    // Init or render array
    triangle_to_render = NULL;

    // Our movement
    mesh.rotation.x += 0.01;
    // mesh.rotation.y += 0.01;
    // mesh.rotation.z += 0.01;
    // mesh.scale.x += 0.002;
    // mesh.scale.y += 0.001;
    // mesh.translation.x += 0.01;
    mesh.translation.z = 5.0;
    
    // Transformation matrices
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);
    // World matrix
    mat4_t world_matrix = mat4_identity();
    world_matrix = mat4_mult(world_matrix, translation_matrix);
    world_matrix = mat4_mult(world_matrix, rotation_matrix_x);
    world_matrix = mat4_mult(world_matrix, rotation_matrix_y);
    world_matrix = mat4_mult(world_matrix, rotation_matrix_z);
    world_matrix = mat4_mult(world_matrix, scale_matrix);

    // Loop over the triangle faces
    for (int i = 0; i < array_length(mesh.faces); i++) {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a];
        face_vertices[1] = mesh.vertices[mesh_face.b];
        face_vertices[2] = mesh.vertices[mesh_face.c];

        vec4_t transformed_vertices[3];

        // Loop over the vertices and apply the transformation and save it for the renderer
        for (int j = 0; j < 3; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);
            transformed_vertex = mat4_mult_vec4(world_matrix, transformed_vertex);
            transformed_vertices[j] = transformed_vertex;
        }

        // Utils for Back culling and light shading                   /*     A     */
        vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);    /*    / \    */
        vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);    /*   /   \   */
        vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);    /*  C --- B  */

        vec3_t vector_ab = vec3_sub(vector_b, vector_a);
        vec3_normalize(&vector_ab);
        vec3_t vector_ac = vec3_sub(vector_c, vector_a);
        vec3_normalize(&vector_ac);

        // Compute the face normal (using cross product to find perpendicular);
        vec3_t normal = vec3_cross(vector_ab, vector_ac);
        // Normalize the vec
        vec3_normalize(&normal);

        // Vector between the a point in the triangle and the camera origin
        vec3_t camera_ray = vec3_sub(camera_position, vector_a);
        // Verify if the normal and camera are align
        float alignment_with_camera = vec3_dot_product(camera_ray, normal);
        if (current_culling_mode == CULLING_ON) {

            // Prune the negative triangle
            if (alignment_with_camera < 0) {
                continue;
            }
        }

        vec4_t projected_points[3];
        // Project the point in 2D
        for (int j = 0; j < 3; j++) {
            // Project the point in 2D
            projected_points[j] = mat4_mul_vec4_project(perspective, transformed_vertices[j]);

            // Scale into view
            projected_points[j].data[0] *= ((float)window_width / 2);
            projected_points[j].data[1] *= ((float)window_height / 2);

            // Invert the y-axis
            projected_points[j].data[1] *= -1;

            // Translate in the middle of the screen
            projected_points[j].data[0] += (float)window_width / 2;
            projected_points[j].data[1] += (float)window_height / 2;
        }

        float avg_depth = (transformed_vertices[0].data[2] + transformed_vertices[1].data[2] + transformed_vertices[2].data[2]) / 3;

        float light_factor = 1.0;
        if (current_light_mode == LIGHT_ON) {
            light_factor = -vec3_dot_product(normal, light.direction);
        }
        
        triangle_t projected_triangle = {
            .avg_depth = avg_depth,
            .color = mesh_face.color,
            .light_intensity = light_factor,
            .points = {
                projected_points[0],
                projected_points[1],
                projected_points[2]
            },
            .tex_coords = {
                { mesh_face.a_uv.u, mesh_face.a_uv.v },
                { mesh_face.b_uv.u, mesh_face.b_uv.v },
                { mesh_face.c_uv.u, mesh_face.c_uv.v },
            }
        };

        // Save the projected tri. for the renderer
        array_push(triangle_to_render, projected_triangle);
    }

    // Bubble sort: will need to be optimized
    int num_triangles = array_length(triangle_to_render);
    for (int i = 0; i < num_triangles; i++) {
        for (int j = i; j < num_triangles; j++) {
            if (triangle_to_render[i].avg_depth < triangle_to_render[j].avg_depth) {
                // Swap the triangles positions in the array
                triangle_t temp = triangle_to_render[i];
                triangle_to_render[i] = triangle_to_render[j];
                triangle_to_render[j] = temp;
            }
        }
    }

}

void render(void) {
    // Clear buffer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    clear_color_buffer(0xFF000000);
    clear_z_buffer();
    draw_ref();

    // Render all the triangle that need to be renderer
    int num_triangles = array_length(triangle_to_render);
    for (int i = 0; i < num_triangles; i++) {
        switch (current_rendering_mode) {
            case WIREFRAME_AND_VERTEX:
            draw_triangle(triangle_to_render[i], triangle_to_render[i].color);
            for (int j = 0; j < 3; j++) {
                draw_rec(triangle_to_render[i].points[j].data[0], triangle_to_render[i].points[j].data[1], 4, 4, COLOR_CONTRAST);
            }
            break;
            case WIREFRAME:
            draw_triangle(triangle_to_render[i], triangle_to_render[i].color);
            break;
            case TRIANGLE:
            draw_filled_triangle(triangle_to_render[i], triangle_to_render[i].color);
            break;
            case TRIANGLE_AND_WIREFRAME:
            draw_triangle(triangle_to_render[i], COLOR_CONTRAST);
            draw_filled_triangle(triangle_to_render[i], triangle_to_render[i].color);
            break;
            case TEXTURE:
                draw_textured_triangle(triangle_to_render[i], mesh_texture);
                break;
            case TEXTURE_AND_WIREFRAME:
                draw_triangle(triangle_to_render[i], COLOR_CONTRAST);
                draw_textured_triangle(triangle_to_render[i], mesh_texture);
                break;
            break;
        }
    }

    // Render
    render_color_buffer();
    SDL_RenderPresent(renderer);
    array_free(triangle_to_render);
}

// Main Function ===============================================================

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
    free_ressources();

    return 0;
}

