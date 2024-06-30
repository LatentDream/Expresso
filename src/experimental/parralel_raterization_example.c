#include "display.h"
#include "vector.h"
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

static int is_running = 0;

int init(void) {
    return 1;
}


/*
*  How to know if the point P is inside the triangle ? 
*
*   V_0
*   |\
*   | \
*   |  \
*   |   \
*   | .  \
*   |  P  \
*   |______\
*   V_2     V_1
*
*   Check if P is to the "right" or the "left" the edges
*   - Using cross product to find the magnitude of the z component (coming out of the screen)
*   - `cross2d = ax * by - bx * ay`
*   - if the arrow point "inside" the triangle, the sign is +
*/

int edge_cross(vec2_t* a, vec2_t* b, vec2_t* p) {
    vec2_t ab = { b->x - a->x, b->y - a->y };
    vec2_t ap = { p->x - a->x, p->y - a->y };
    return ab.x * ap.y - ab.y * ap.x;
}

int is_top_left(vec2_t* start, vec2_t* end) {
    vec2_t edge = {end->x - start->x, end->y - start->y};
    int is_top_edge = edge.y == 0 && edge.x > 0;
    int is_left_edge = edge.y < 0;
    return is_top_edge || is_left_edge;
}

void triangle_fill(vec2_t v0, vec2_t v1, vec2_t v2, color_t color) {
    // PERF: Edge function and constant increment so when we go out of the triangle, we go to the next loop
    // See: https://www.cs.drexel.edu/~deb39/Classes/Papers/comp175-06-pineda.pdf
    int x_min = MIN(v0.x, MIN(v1.x, v2.x));
    int y_min = MIN(v0.y, MIN(v1.y, v2.y));
    int x_max = MAX(v0.x, MAX(v1.x, v2.x));
    int y_max = MAX(v0.y, MAX(v1.y, v2.y));
    
    // Top-Left Rasterization Rule
    int bias0 = is_top_left(&v1, &v2) ? 0 : -1;
    int bias1 = is_top_left(&v2, &v0) ? 0 : -1;
    int bias2 = is_top_left(&v0, &v1) ? 0 : -1;

    for (int y = y_min; y <= y_max; y++) {
        for (int x = x_min; x <= x_max; x++) {
            vec2_t p = {x, y};

            // Check if the pixel is inside the triangle
            int w0 = edge_cross(&v1, &v2, &p) + bias0;
            int w1 = edge_cross(&v2, &v0, &p) + bias1;
            int w2 = edge_cross(&v0, &v1, &p) + bias2;
            int is_inside = w0 >= 0 && w1 >= 0 && w2 >= 0;

            if (is_inside) {
                draw_pixel(x, y, color);
            }
        }
    }
}

int main_example(void) {

    is_running = init();

    // A triangle in screen space
    vec2_t vertices[3] = {
        {.x = 10, .y = 2 },
        {.x = 20, .y = 28},
        {.x = 3,  .y = 21}
    };

    // Find a bounding box with all the condidate pixels
    vec2_t v0 = vertices[0];
    vec2_t v1 = vertices[1];
    vec2_t v2 = vertices[2];

    triangle_fill(v0, v1, v2, 0xFF00FF00);

    return 0;
}
