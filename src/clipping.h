#ifndef CLIPPING_H
#define CLIPPING_H

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

#define MAX_NUM_VERTICES 8 
typedef struct {
    vec3_t vertices[MAX_NUM_VERTICES];
    int num_vertices;
} polygon_t;


polygon_t create_polygon(vec3_t* triangle_vertices);
void clip_polygon(polygon_t* polygon);

void initialize_frustum_planes(float fov, float z_near, float z_far);

#endif // !CLIPPING_H
