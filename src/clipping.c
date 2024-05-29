#include "clipping.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

plane_t frustplanes[6] = {0};

void initialize_frustum_planes(float fov, float z_near, float z_far) {
    float cos_half_fov = cos(fov / 2);
    float sin_half_fov = sin(fov / 2);

    vec3_t origin = {0, 0, 0};

    frustplanes[LEFT_FRUSTUM_PLANE].point = origin;
    frustplanes[LEFT_FRUSTUM_PLANE].normal = (vec3_t) {cos_half_fov, 0, sin_half_fov};

    frustplanes[RIGHT_FRUSTUM_PLANE].point = origin;
    frustplanes[RIGHT_FRUSTUM_PLANE].normal = (vec3_t) {-cos_half_fov, 0, sin_half_fov};

    frustplanes[TOP_FRUSTUM_PLANE].point = origin;
    frustplanes[TOP_FRUSTUM_PLANE].normal = (vec3_t) {0, -cos_half_fov, sin_half_fov};

    frustplanes[BOTTOM_FRUSTUM_PLANE].point = origin;
    frustplanes[BOTTOM_FRUSTUM_PLANE].normal = (vec3_t) {0, cos_half_fov, sin_half_fov};

    frustplanes[NEAR_FRUSTUM_PLANE].point = (vec3_t) {0, 0, z_near};
    frustplanes[NEAR_FRUSTUM_PLANE].normal = (vec3_t) {0, 0, 1};

    frustplanes[FAR_FRUSTUM_PLANE].point = (vec3_t) {0, 0, z_far};
    frustplanes[FAR_FRUSTUM_PLANE].normal = (vec3_t) {0, 0, -1};
}


void clip_polygon_against_plane(polygon_t* polygon, int plane_index) {
    vec3_t plane_point = frustplanes[plane_index].point;
    vec3_t plane_normal = frustplanes[plane_index].normal;

    // Generate a new list of vertices for the clipped polygon
    vec3_t inside_vertices[MAX_NUM_VERTICES];
    int num_inside_vertices = 0;

    vec3_t* current_vertex  = &polygon->vertices[0];
    vec3_t* previous_vertex = &polygon->vertices[polygon->num_vertices - 1];

    float current_dot; // dot_Q_1 = n * (Q_1 - P) | if + then Q_1 is **inside** the plane
    float previous_dot = vec3_dot_product(vec3_sub(*previous_vertex, plane_point), plane_normal);

    // Loop while the current vertex is diffent than the last vertex
    while (current_vertex != &polygon->vertices[polygon->num_vertices]) {
        current_dot = vec3_dot_product(vec3_sub(*current_vertex, plane_point), plane_normal);

        // If current is inside the plane && previous is outside the plane (or vice versa)
        if (current_dot * previous_dot < 0) {
            // TODO: Test this code
            // Calculate the intersection point
            float t = previous_dot / (previous_dot - current_dot);
            vec3_t intersection_point = vec3_add(
                *previous_vertex,
                vec3_mult(vec3_sub(*current_vertex, *previous_vertex), t));

            // Insert the new intersection point
            inside_vertices[num_inside_vertices] = vec3_clone(&intersection_point);
            num_inside_vertices++;
        }

        // If inside the plane:
        if (current_dot > 0) {
            inside_vertices[num_inside_vertices] = vec3_clone(current_vertex);
            num_inside_vertices++;
        }

        // Next vertex
        previous_dot = current_dot;
        previous_vertex = current_vertex;
        current_vertex++;
    }
    // Copy the inside vertices to the polygon
    for (int i = 0; i < num_inside_vertices; i++) {
        polygon->vertices[i] = vec3_clone(&inside_vertices[i]);
    }
    polygon->num_vertices = num_inside_vertices;

}

void clip_polygon(polygon_t* polygon) {
    clip_polygon_against_plane(polygon, LEFT_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, TOP_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, NEAR_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, FAR_FRUSTUM_PLANE);
}

polygon_t create_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2) {
    polygon_t polygon = {
        .vertices = { v0, v1, v2 },
        .num_vertices = 3
    };
    return polygon;
}
