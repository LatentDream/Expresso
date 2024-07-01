#ifndef MESH_H
#define MESH_H

#include "display.h"
#include "upng.h"
#include "vector.h"
#include "triangle.h"

// This would be equivalent of a "Game Object"
typedef struct {
    vec3_t* vertices;  // Dynamic array of vertices   |
    face_t* faces;     // Dynamic array of faces      |
    upng_t *texture;   // Texture for the mesh        |
    vec3_t rotation;   // Rotation with xyz value     |
    vec3_t scale;      // Scale with xyz value        |
    vec3_t translation;// Translation with xyz value  |
} mesh_t;

void load_mesh(char* obj_filename, char* png_filename, vec3_t scaling, vec3_t translation, vec3_t rotation);
void load_mesh_and_data_from_obj(mesh_t* mesh, char* filename);
void load_mesh_png_texture(mesh_t* mesh, char* filename);
int get_num_meshes();
mesh_t* get_mesh(int mesh_idx);
void free_meshes();

#endif // !MESH_H
