#include "clipping.h"
#include "texture.h"
#include "triangle.h"
#include "upng.h"
#include "vector.h"
#include <math.h>

float float_lerp(float a, float b, float t) {
    return a + t * (b - a);
}

plane_t frustplanes[6] = {0};

void initialize_frustum_planes(float fovy, float fovx, float z_near, float z_far) {
    float cos_half_fovy = cos(fovy / 2);
    float sin_half_fovy = sin(fovy / 2);
    float cos_half_fovx = cos(fovx / 2);
    float sin_half_fovx = sin(fovx / 2);

    vec3_t origin = {0, 0, 0};

    frustplanes[LEFT_FRUSTUM_PLANE].point = origin;
    frustplanes[LEFT_FRUSTUM_PLANE].normal = (vec3_t) {cos_half_fovx, 0, sin_half_fovx};

    frustplanes[RIGHT_FRUSTUM_PLANE].point = origin;
    frustplanes[RIGHT_FRUSTUM_PLANE].normal = (vec3_t) {-cos_half_fovx, 0, sin_half_fovx};

    frustplanes[TOP_FRUSTUM_PLANE].point = origin;
    frustplanes[TOP_FRUSTUM_PLANE].normal = (vec3_t) {0, -cos_half_fovy, sin_half_fovy};

    frustplanes[BOTTOM_FRUSTUM_PLANE].point = origin;
    frustplanes[BOTTOM_FRUSTUM_PLANE].normal = (vec3_t) {0, cos_half_fovy, sin_half_fovy};

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
    tex2_t inside_tex_coords[MAX_NUM_VERTICES];
    int num_inside_vertices = 0;

    vec3_t* current_vertex  = &polygon->vertices[0];
    tex2_t* current_texture = &polygon->tex_coords[0];
    vec3_t* previous_vertex = &polygon->vertices[polygon->num_vertices - 1];
    tex2_t* previous_texture = &polygon->tex_coords[polygon->num_vertices - 1];

    float current_dot; // dot_Q_1 = n * (Q_1 - P) | if + then Q_1 is **inside** the plane
    float previous_dot = vec3_dot_product(vec3_sub(*previous_vertex, plane_point), plane_normal);

    // Loop while the current vertex is diffent than the last vertex
    while (current_vertex != &polygon->vertices[polygon->num_vertices]) {
        current_dot = vec3_dot_product(vec3_sub(*current_vertex, plane_point), plane_normal);

        // If current is inside the plane && previous is outside the plane (or vice versa)
        if (current_dot * previous_dot < 0) {
            float t = previous_dot / (previous_dot - current_dot);
            // Calculate the intersection point I = Q1 + t(Q2-Q1)
            vec3_t intersection_point = {
                .x = float_lerp(previous_vertex->x, current_vertex->x, t),
                .y = float_lerp(previous_vertex->y, current_vertex->y, t),
                .z = float_lerp(previous_vertex->z, current_vertex->z, t)
            };

            // Use the lerp formula to get the interpolated U and V texture coordinates
            tex2_t interpolated_texcoord = {
                .u = float_lerp(previous_texture->u, current_texture->u, t),
                .v = float_lerp(previous_texture->v, current_texture->v, t)
            };

            // Insert the new intersection point
            inside_vertices[num_inside_vertices] = vec3_clone(&intersection_point);
            inside_tex_coords[num_inside_vertices] = tex2_clone(&interpolated_texcoord);
            num_inside_vertices++;
        }

        // If inside the plane:
        if (current_dot > 0) {
            inside_vertices[num_inside_vertices] = vec3_clone(current_vertex);
            inside_tex_coords[num_inside_vertices] = tex2_clone(current_texture);
            num_inside_vertices++;
        }

        // Next vertex
        previous_dot = current_dot;
        previous_vertex = current_vertex;
        previous_texture = current_texture;
        current_vertex++;
        current_texture++;
    }
    // Copy the inside vertices to the polygon
    for (int i = 0; i < num_inside_vertices; i++) {
        polygon->vertices[i] = vec3_clone(&inside_vertices[i]);
        polygon->tex_coords[i] = tex2_clone(&inside_tex_coords[i]);
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

polygon_t create_polygon_from_triangle(
    vec3_t v0, vec3_t v1, vec3_t v2,
    tex2_t uv0, tex2_t uv1, tex2_t uv2
) {
    polygon_t polygon = {
        .vertices = { v0, v1, v2 },
        .num_vertices = 3,
        .tex_coords = { uv0, uv1, uv2 }
    };
    return polygon;
}

void create_triangles_from_polygon(
    polygon_t* polygon,
    triangle_t* clipped_triangles,
    int* num_clipped_triangles,
    upng_t* texture
) {
     for (int i = 0; i < polygon->num_vertices - 2; i++) {
         int index0 = 0;
         int index1 = i + 1;
         int index2 = i + 2;

         clipped_triangles[i].points[0] = vec4_from_vec3(polygon->vertices[index0]);
         clipped_triangles[i].points[1] = vec4_from_vec3(polygon->vertices[index1]);
         clipped_triangles[i].points[2] = vec4_from_vec3(polygon->vertices[index2]);
         clipped_triangles[i].tex_coords[0] = polygon->tex_coords[index0];
         clipped_triangles[i].tex_coords[1] = polygon->tex_coords[index1];
         clipped_triangles[i].tex_coords[2] = polygon->tex_coords[index2];
         clipped_triangles[i].texture = texture;
     }
     *num_clipped_triangles = polygon->num_vertices - 2;
}
