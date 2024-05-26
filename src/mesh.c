#include "mesh.h"
#include "array.h"
#include "display.h"
#include "vector.h"
#include <stdio.h>
#include <string.h>

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = { 0, 0, 0 },
    .scale = { 1.0, 1.0, 1.0 },
    .translation = { 0, 0, 0 }
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    { .x = -1, .y = -1, .z = -1},   // 1
    { .x = -1, .y =  1, .z = -1},   // 2
    { .x =  1, .y =  1, .z = -1},   // 3
    { .x =  1, .y = -1, .z = -1},   // 4
    { .x =  1, .y =  1, .z =  1},   // 5
    { .x =  1, .y = -1, .z =  1},   // 6
    { .x = -1, .y =  1, .z =  1},   // 7
    { .x = -1, .y = -1, .z =  1},   // 8
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    { .a = 1, .b = 2, .c = 3, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 1, .b = 3, .c = 4, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    // right
    { .a = 4, .b = 3, .c = 5, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 4, .b = 5, .c = 6, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    // back
    { .a = 6, .b = 5, .c = 7, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 6, .b = 7, .c = 8, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    // left
    { .a = 8, .b = 7, .c = 2, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 8, .b = 2, .c = 1, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    // top
    { .a = 2, .b = 7, .c = 5, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 2, .b = 5, .c = 3, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    // bottom
    { .a = 6, .b = 8, .c = 1, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 6, .b = 1, .c = 4, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF }
};

void load_cube_example_mesh(void) {
    for (int i = 0; i < N_CUBE_VERTICES; i++) {
        array_push(mesh.vertices, cube_vertices[i])
    }
    for (int i = 0; i < N_CUBE_FACES; i++) {
        array_push(mesh.faces, cube_faces[i])
    }
}

// Load a mesh from an .obj file that contains only vertices and faces
void load_mesh_from_obj_simple(const char* filename, color_t color) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open file: %s\n", filename);
        return;
    }
    char line[128];
    while (fgets(line, sizeof(line), file)) {
        vec3_t vertex;
        if (line[0] == 'v') {
            sscanf(line, "v %f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
        } else if (line[0] == 'f') {
            face_t face;
            sscanf(line, "f %d %d %d\n", &face.a, &face.b, &face.c);
            face.color = color;
            array_push(mesh.faces, face);
        }
    }
    fclose(file);

}

void load_mesh_from_obj_complex(char* filename, color_t color) {
    FILE* file;
    file = fopen(filename, "r");
    char line[1024];

    while (fgets(line, 1024, file)) {
        // Vertex information
        if (strncmp(line, "v ", 2) == 0) {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
        }
        // Face information
        if (strncmp(line, "f ", 2) == 0) {
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];
            sscanf(
                line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &vertex_indices[0], &texture_indices[0], &normal_indices[0], 
                &vertex_indices[1], &texture_indices[1], &normal_indices[1], 
                &vertex_indices[2], &texture_indices[2], &normal_indices[2]
            ); 
            face_t face = {
                .a = vertex_indices[0],
                .b = vertex_indices[1],
                .c = vertex_indices[2],
                .color = color
            };
            array_push(mesh.faces, face);
        }
    }
}
