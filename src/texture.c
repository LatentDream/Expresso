#include "texture.h"

tex2_t tex2_clone(tex2_t* tex) {
    return (tex2_t) {tex->u, tex->v};
}

