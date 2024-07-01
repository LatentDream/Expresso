#include "mesh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_mesh_and_texture(char* filename, vec3_t scaling, vec3_t position, vec3_t rotation) {
    char* obj_filename = malloc(strlen(filename) + 5);
    char* png_filename = malloc(strlen(filename) + 5);
    sprintf(obj_filename, "%s.obj", filename);
    sprintf(png_filename, "%s.png", filename);
    load_mesh(obj_filename, png_filename, scaling, position, rotation);
    free(obj_filename);
    free(png_filename);
}

void load_entity(char* filename, vec3_t scaling, vec3_t position, vec3_t rotation) {
    printf("[INFO] Loading entity: %s\n", filename);
    load_mesh_and_texture(filename,scaling,position,rotation);
    // TODO: init entity struct
}

void load_prop(char* filename, vec3_t scaling, vec3_t position, vec3_t rotation) {
    printf("[INFO] Loading prop: %s\n", filename);
    load_mesh_and_texture(filename,scaling,position,rotation);
}
