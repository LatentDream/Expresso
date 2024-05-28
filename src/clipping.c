#include "clipping.h"
#include "vector.h"
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
