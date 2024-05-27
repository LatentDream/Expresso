#ifndef MESH_H
#define MESH_H

#include "display.h"
#include "vector.h"
#include "triangle.h"

// For the cube example

#define N_CUBE_VERTICES 8
extern vec3_t cube_vertices[N_CUBE_VERTICES];

#define N_CUBE_FACES (6 * 2)
extern face_t cube_faces[N_CUBE_FACES];

// Dynamic mesh ---------------------------------------
typedef struct {
    vec3_t* vertices;  // Dynamic array of vertices   |
    face_t* faces;     // Dynamic array of faces      |
    vec3_t rotation;   // Rotation with xyz value     |
    vec3_t scale;      // Scale with xyz value        |
    vec3_t translation;// Translation with xyz value  
} mesh_t;

extern mesh_t mesh;

void load_cube_example_mesh(void);
void load_mesh_from_obj_simple(const char* filename, color_t color);
void load_mesh_and_texture_from_obj(char* filename, color_t color);

#endif // !MESH_H
