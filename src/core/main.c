#include <SDL2/SDL_events.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "array.h"
#include "camera.h"
#include "clipping.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "texture.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"
#include "quattro.h"

// Event Loop
bool is_running = false;
int previous_frame_time = 0;
float delta_time = 0;

// Meshes
#define MAX_TRIANGLES_PER_MESH 20000
triangle_t triangle_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

// Matrices
mat4_t view_matrix;
mat4_t world_matrix;

// Modes
static color_t COLOR_CONTRAST = 0xFF1154BB;
mat4_t perspective;

#define PI 3.14159265

// Control
float previous_mouse_x = 0.0;
float previous_mouse_y = 0.0;
const int MOUSE_SENSITIVITY = 40;  // The more, the less sensitive

// Setup Function ==============================================================

void setup(void) {

    set_render_mode(TRIANGLE_AND_WIREFRAME);
    set_current_light_mode(LIGHT_ON);
    set_culling_mode(CULLING_ON);

    create_light((vec3_t){0, 0, 1});

    // Init the perspective matrix
    float aspecty = (float)get_window_height() / (float)get_window_width();
    float aspectx = (float)get_window_width()  / (float)get_window_height();
    float fovy = 1.047194;  // 60 degrees in radians
    float fovx = 2 * atan(aspectx * tan(fovy / 2));
    float near = 0.1;
    float far = 100.0;
    perspective = mat4_make_perspective(fovy, aspecty, near, far);

    initialize_frustum_planes(fovy, fovx, near, far);

    // TODO: Move this into the Entity System
    char* filename = "./assets/planes/f22";
    load_mesh(filename, (vec3_t){1, 1, 1}, (vec3_t){0, -1.3, +5}, (vec3_t){0, -PI/2, 0});

    filename = "./assets/planes/f117";
    load_mesh(filename, (vec3_t){1, 1, 1}, (vec3_t){2, -1.3, +9}, (vec3_t){0, -PI/2, 0});

    filename = "./assets/planes/efa";
    load_mesh(filename, (vec3_t){1, 1, 1}, (vec3_t){-2, -1.3, +9}, (vec3_t){0, -PI/2, 0});

    filename = "./assets/planes/runway";
    load_mesh(filename, (vec3_t){1, 0, 1}, (vec3_t){0, -1.5, +23}, (vec3_t){0, 0, 0});

    uint64_t result = add_from_rust(20, 22);
    printf("Result: %ld\n", result);

}

void free_ressources(void) {
    free_meshes();
}

// Process Input Function ======================================================

void process_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {

        switch (event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_KEYDOWN:
                // Quit ---------------------------
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running = false;
                    break;
                }
                // Rendering mode -----------------
                if (event.key.keysym.sym == SDLK_c) {
                    set_culling_mode((get_culling_mode() + 1) % 2);
                    break;
                }
                if (event.key.keysym.sym == SDLK_y) {
                    set_render_mode(WIREFRAME_AND_VERTEX);
                    break;
                }
                if (event.key.keysym.sym == SDLK_u) {
                    set_render_mode(WIREFRAME);
                    break;
                }
                if (event.key.keysym.sym == SDLK_i) {
                    set_render_mode(TRIANGLE);
                    break;
                }
                if (event.key.keysym.sym == SDLK_o) {
                    set_render_mode(TRIANGLE_AND_WIREFRAME);
                    break;
                }
                if (event.key.keysym.sym == SDLK_t) {
                    set_render_mode(TEXTURE);
                    break;
                }
                if (event.key.keysym.sym == SDLK_g) {
                    set_render_mode(TEXTURE_AND_WIREFRAME);
                    break;
                }
                // Light mode ---------------------
                if (event.key.keysym.sym == SDLK_l) {
                    set_current_light_mode((get_current_light_mode() + 1) % 2);
                    break;
                }

                // Camera movement ----------------
                if (event.key.keysym.sym == SDLK_a) {
                    // TODO: Move the camera to the side
                    break;
                }
                if (event.key.keysym.sym == SDLK_d) {
                    // TODO: Move the camera to the side
                    break;
                }
                if (event.key.keysym.sym == SDLK_w) {
                    update_camera_forward_velocity(vec3_mult(get_camera_direction(), 5.0 * delta_time));
                    update_camera_position(vec3_add(get_camera_position(), get_camera_forward_velocity()));
                    break;
                }
                if (event.key.keysym.sym == SDLK_s) {
                    update_camera_forward_velocity(vec3_mult(get_camera_direction(), 5.0 * delta_time));
                    update_camera_position(vec3_sub(get_camera_position(), get_camera_forward_velocity()));
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                break;
            case SDL_MOUSEMOTION:
                SDL_MouseMotionEvent * mov = (SDL_MouseMotionEvent*)&event;
                const float x_delta = (previous_mouse_x - (float)mov->xrel) / MOUSE_SENSITIVITY;
                const float y_delta = (previous_mouse_y - (float)mov->yrel) / MOUSE_SENSITIVITY;
                previous_mouse_x = (float)mov->xrel;
                previous_mouse_y = (float)mov->yrel;
                if (fabs(x_delta) <= 10 && fabs(y_delta) <= 10) {
                    rotate_camera_yaw(x_delta * delta_time);
                    rotate_camera_pitch(y_delta  * delta_time);
                }
                break;
        }
    }
}

// Update Function =============================================================


/* 
* Graphic Pipeline
* ----------------
* Model Space
*  |_ World Space
*     |_ Camera Space
*        |_ Clipping
*           |_ Projection
*              |_ Image Space (with perspective divide)
*                 |_ Screen Space
*/
void process_graphic_pipeline(mesh_t* mesh) {
    // MOVEMENT OF CAMERA -----------------------------------------------------
    vec3_t target = get_camera_lookat_target();
    view_matrix = mat4_look_at(get_camera_position(), target);
    
    // MODEL SPACE -> WORLD SPACE ---------------------------------------------
    // Transformation matrices
    mat4_t scale_matrix = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh->rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh->rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh->rotation.z);
    // World matrix
    world_matrix = mat4_identity();
    world_matrix = mat4_mult(world_matrix, translation_matrix);
    world_matrix = mat4_mult(world_matrix, rotation_matrix_x);
    world_matrix = mat4_mult(world_matrix, rotation_matrix_y);
    world_matrix = mat4_mult(world_matrix, rotation_matrix_z);
    world_matrix = mat4_mult(world_matrix, scale_matrix);   //

    for (int i = 0; i < array_length(mesh->faces); i++) {
        face_t mesh_face = mesh->faces[i];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh->vertices[mesh_face.a];
        face_vertices[1] = mesh->vertices[mesh_face.b];
        face_vertices[2] = mesh->vertices[mesh_face.c];

        vec4_t transformed_vertices[3];

        // Loop over the vertices and apply the transformation and save it for the renderer
        for (int j = 0; j < 3; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);
            transformed_vertex = mat4_mult_vec4(world_matrix, transformed_vertex);
            // WORLD SPACE -> CAMERA SPACE -------------------------------------
            transformed_vertex = mat4_mult_vec4(view_matrix, transformed_vertex);
            transformed_vertices[j] = transformed_vertex;
        }

        // CULLING -----------------------------------------------------------
        // Find the normal
        vec3_t normal = get_triangle_normal(transformed_vertices);
        // Vector between the a point in the triangle and the origin
        vec3_t camera_ray = vec3_sub((vec3_t){0, 0, 0}, vec3_from_vec4(transformed_vertices[0]));
        // Verify if the normal and camera are align
        float alignment_with_camera = vec3_dot_product(camera_ray, normal);
        if (get_culling_mode() == CULLING_ON) {
            // Prune the negative triangle
            if (alignment_with_camera < 0) {
                continue;
            }
        }

        // CLIPPING -----------------------------------------------------------
        // Create a polygon from the triangle
        polygon_t polygon = create_polygon_from_triangle(
            vec3_from_vec4(transformed_vertices[0]),
            vec3_from_vec4(transformed_vertices[1]),
            vec3_from_vec4(transformed_vertices[2]),
            mesh_face.a_uv,
            mesh_face.b_uv,
            mesh_face.c_uv
        );
        clip_polygon(&polygon);
        // Create a triangle from the polygon
        triangle_t clipped_triangles[MAX_NUM_TRIANGLES];
        int num_clipped_triangles = 0;
        create_triangles_from_polygon(&polygon, clipped_triangles, &num_clipped_triangles, mesh->texture);

        // Only render the triangle that are inside the frustum
        for (int t = 0; t < num_clipped_triangles; t++) {
            triangle_t clipped_triangle = clipped_triangles[t];

            // PROJECTION -----------------------------------------------------
            vec4_t projected_points[3];
            // Project the point in 2D
            for (int j = 0; j < 3; j++) {
                // IMAGE SPACE ------------------------------------------------
                // Project the point in 2D
                projected_points[j] = mat4_mul_vec4_project(perspective, clipped_triangle.points[j]);

                // SCREEN SPACE -----------------------------------------------
                // Scale into view
                projected_points[j].data[0] *= ((float)get_window_width() / 2);
                projected_points[j].data[1] *= ((float)get_window_height() / 2);

                // Invert the y-axis
                projected_points[j].data[1] *= -1;

                // Translate in the middle of the screen
                projected_points[j].data[0] += (float)get_window_width() / 2;
                projected_points[j].data[1] += (float)get_window_height() / 2;
            }

            float light_factor = 1.0;
            if (get_current_light_mode() == LIGHT_ON) {
                light_factor = -vec3_dot_product(normal, get_light().direction);
            }
            
            triangle_t projected_triangle = {
                .color = mesh_face.color,
                .light_intensity = light_factor,
                .points = {
                    projected_points[0],
                    projected_points[1],
                    projected_points[2]
                },
                .tex_coords = {
                    { clipped_triangle.tex_coords[0].u, clipped_triangle.tex_coords[0].v },
                    { clipped_triangle.tex_coords[1].u, clipped_triangle.tex_coords[1].v },
                    { clipped_triangle.tex_coords[2].u, clipped_triangle.tex_coords[2].v },
                },
                .texture = mesh->texture
            };

            // Save the projected tri. for the renderer
            triangle_to_render[num_triangles_to_render++] = projected_triangle;
        }
    }
}


/* 
* Update Each "Objects" and pass them to the graphic pipeline
*/
void update(void) {
    // Limit the tick to the FRAME_TARGET_TIME
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    while (!SDL_TICKS_PASSED(
        SDL_GetTicks(),
        previous_frame_time + FRAME_TARGET_TIME))
    {
        SDL_Delay(time_to_wait);
    }
    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0; // For update game object
    previous_frame_time = SDL_GetTicks();

    // Init or render array
    num_triangles_to_render = 0;

    for (int mesh_idx = 0; mesh_idx < get_num_meshes(); mesh_idx++) { 

        mesh_t* mesh = get_mesh(mesh_idx);
        if (mesh == NULL) {
            printf("[ERROR] Mesh %d is NULL\n", mesh_idx);
            continue;
        }

        // MOVEMENT OF OBJECT -------------------------------------------------
        // mesh->rotation.x += 0.4 * delta_time;
        // mesh->rotation.y += 0.2 * delta_time;
        // mesh->rotation.z += 0.1 * delta_time;
        // mesh->scale.x += 0.02 * delta_time;
        // mesh->scale.y += 0.01 * delta_time;
        // mesh->translation.z += 0.01 * delta_time;
        // mesh->translation.z = 5.0;

        process_graphic_pipeline(mesh);

    }
}

/*
* Render the triangle to the screen
*/
void render(void) {
    // Clear buffer
    clear_color_buffer(0xFF000000);
    clear_z_buffer();
    draw_ref();

    // Render all the triangle that need to be renderer
    for (int i = 0; i < num_triangles_to_render; i++) {
        switch (get_render_mode()) {
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
                draw_textured_triangle(triangle_to_render[i]);
                break;
            case TEXTURE_AND_WIREFRAME:
                draw_triangle(triangle_to_render[i], COLOR_CONTRAST);
                draw_textured_triangle(triangle_to_render[i]);
                break;
            break;
        }
    }

    // Render
    render_color_buffer();
}

// Main Function ===============================================================

int main(int argc, char *argv[]) {

    // Create SDL window
    is_running = initialize_window(false, false);

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

