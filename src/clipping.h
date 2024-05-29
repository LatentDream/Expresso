#ifndef CLIPPING_H
#define CLIPPING_H

#include "texture.h"
#include "triangle.h"
#include "vector.h"
enum {
    LEFT_FRUSTUM_PLANE,
    RIGHT_FRUSTUM_PLANE,
    TOP_FRUSTUM_PLANE,
    BOTTOM_FRUSTUM_PLANE,
    NEAR_FRUSTUM_PLANE,
    FAR_FRUSTUM_PLANE
};

typedef struct{
    vec3_t point;
    vec3_t normal;
} plane_t;

#define MAX_NUM_VERTICES 10 
#define MAX_NUM_TRIANGLES MAX_NUM_VERTICES - 2
typedef struct {
    vec3_t vertices[MAX_NUM_VERTICES];
    tex2_t tex_coords[MAX_NUM_VERTICES];
    int num_vertices;
} polygon_t;


polygon_t create_polygon(vec3_t* triangle_vertices);
void clip_polygon(polygon_t* polygon);

void initialize_frustum_planes(float fovy, float fovx, float z_near, float z_far);
polygon_t create_polygon_from_triangle(
    vec3_t v0, vec3_t v1, vec3_t v2,
    tex2_t uv0, tex2_t uv1, tex2_t uv2);
void create_triangles_from_polygon(polygon_t* polygon, triangle_t* clipped_triangles, int* num_clipped_triangles);


#endif // !CLIPPING_H
