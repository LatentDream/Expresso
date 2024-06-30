#include "light.h"
#include "display.h"

static light_t light = {
    .direction = { 0, 0, 1 }
};

color_t shade_color(color_t original_color, float factor) {
    if (factor < 0) factor = 0;
    if (factor > 1) factor = 1;

    // Shade the color
    color_t a = (original_color & 0xFF000000);
    color_t r = (original_color & 0x00FF0000) * factor;
    color_t g = (original_color & 0x0000FF00) * factor;
    color_t b = (original_color & 0x000000FF) * factor;
    color_t result = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);

    return result;
}

light_t create_light(vec3_t direction) {
    light_t result = {
        .direction = direction
    };
    return result;
}

light_t get_light(void) {
    return light;
}

