#include "mesh.h"
#include "array.h"
#include "texture.h"
#include "upng.h"
#include "vector.h"
#include <stdio.h>
#include <string.h>

#define MAX_MESHES 256
static mesh_t meshes[MAX_MESHES];
static int num_meshes = 0;

void load_mesh(char* obj_filename, char* png_filename, vec3_t scaling, vec3_t translation, vec3_t rotation) {
    load_mesh_and_data_from_obj(&meshes[num_meshes], obj_filename);
    load_mesh_png_texture(&meshes[num_meshes], png_filename);
    meshes[num_meshes].scale = scaling;
    meshes[num_meshes].translation = translation;
    meshes[num_meshes].rotation = rotation;
    num_meshes++;
}

void free_meshes() {
    for (int i = 0; i < num_meshes; i++) {
        array_free(meshes[i].vertices);
        array_free(meshes[i].faces);
        upng_free(meshes[i].texture);
    }
    num_meshes = 0;
}


void load_mesh_and_data_from_obj(mesh_t* mesh, char* filename) {
    FILE* file;
    file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return;
    }
    char line[1024];

    tex2_t* texcoords = NULL;

    while (fgets(line, 1024, file)) {
        // Vertex information
        if (strncmp(line, "v ", 2) == 0) {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh->vertices, vertex);
        }

        // Texture information
        if (strncmp(line, "vt ", 3) == 0) {
            tex2_t texcoord;
            sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v);
            texcoord.v = 1.0 - texcoord.v;
            array_push(texcoords, texcoord);
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
                .a = vertex_indices[0] - 1,
                .b = vertex_indices[1] - 1,
                .c = vertex_indices[2] - 1,
                .a_uv = texcoords[texture_indices[0]-1],
                .b_uv = texcoords[texture_indices[1]-1],
                .c_uv = texcoords[texture_indices[2]-1],
                .color = 0xFFEEEEEE
            };
            array_push(mesh->faces, face);
        }
    }

    array_free(texcoords);
}

void load_mesh_png_texture(mesh_t* mesh, char* filename) {
    upng_t* upng_image = upng_new_from_file(filename);
    if (upng_image != NULL) {
        upng_decode(upng_image);
        if (upng_get_error(upng_image) == UPNG_EOK) {
            mesh->texture = upng_image;
        } else {
            fprintf(stderr, "Error decoding PNG\n");
        }
    } else {
        fprintf(stderr, "Error loading PNG: %s\n", filename);
    }
}

int get_num_meshes() {
    return num_meshes;
}

mesh_t* get_mesh(int mesh_idx) {
    if (mesh_idx < 0 || mesh_idx >= num_meshes) {
        return NULL;
    }
    return &meshes[mesh_idx];
}

// TODO: Make this compatible Load a mesh from an .obj file that contains only vertices and faces
void load_mesh_from_obj(mesh_t* mesh, char* filename) {
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
            array_push(mesh->vertices, vertex);
        } else if (line[0] == 'f') {
            face_t face;
            sscanf(line, "f %d %d %d\n", &face.a, &face.b, &face.c);
            face.color = 0xFFEEEEEE;
            face.a_uv = (tex2_t){ 0, 0 };
            face.b_uv = (tex2_t){ 0, 0 };
            face.c_uv = (tex2_t){ 0, 0 };

            array_push(mesh->faces, face);

        }
    }
    fclose(file);

}

