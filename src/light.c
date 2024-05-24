#include "light.h"
#include "display.h"

light_t light = {
    .direction = { 0, 0, 1 }
};

color_t shade_color(color_t original_color, float factor) {

    // Shade the color
    color_t a = (original_color & 0xFF000000);
    color_t r = (original_color & 0x00FF0000) * factor;
    color_t g = (original_color & 0x0000FF00) * factor;
    color_t b = (original_color & 0x000000FF) * factor;
    color_t result = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);

    return result;
}