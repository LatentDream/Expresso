# Expresso

Old school 3D engine.

## Dev Env

```
sudo apt install build-essential
sudo apt install libsdl2-dev
```

---

## Notes


3D points:
```
y
|
|   z   . (2, 2, 1)
|  /   
| /
|/________ x
```

**Rotation**
REM: SOH CAH TOA ;)
```
Rotation is the following
- We have:
    x = r cos(a) and y = r sin (a)
- We want
    x' = r cos(a+b) and y' = r sin(a+b)

- Then with the angle addition formula for cosine
x' = r cos(a + b)
x' = r (cos(a) cos(b) - sin(a) sin(b))
x' = r cos(a) cos(b) - r sin(a) sin(b)
x' = x cos(b) - y sin(b)

So then the matrix will be:
[cos(a) -sin(a)] [x]
[sin(a)  cos(a)] [y]

And extending to the next dimention:

[cos(a) -sin(a)  0 ] [x]
[sin(a)  cos(a)  0 ] [y]
[ 0       0      1 ] [z]
```

**3D Mesh**
Each triangle has `3 vertex` (corners)

Let's check the code for a cube
```C
#include "vector.h"

// those are points
vec3_t cube_vertices[8] = {
    { .x = -1, .y = -1, .z = -1},   // 1
    { .x = -1, .y =  1, .z = -1},   // 2
    { .x =  1, .y =  1, .z = -1},   // 3
    { .x =  1, .y = -1, .z = -1},   // 4
    { .x =  1, .y =  1, .z =  1},   // 5
    { .x =  1, .y = -1, .z =  1},   // 6
    { .x = -1, .y =  1, .z =  1},   // 7
    { .x = -1, .y = -1, .z =  1},   // 8
};

// for a face (triangle)
typedef struct {
    int a, b, c;
}  face_t;

// the collection of triangle for the cube
//   warning: the order matter because 
//   we want a front and a back face
//
//   here: we do a clock wise orientation
face_t cube_faces[12] = {
    // front
    { .a = 1, .b = 2, .c = 3},
    { .a = 1, .b = 3, .c = 4},
    // right
    { .a = 4, .b = 3, .c = 5},
    { .a = 4, .b = 5, .c = 6},
    // back
    { .a = 6, .b = 5, .c = 7},
    { .a = 6, .b = 7, .c = 8},
    // left
    { .a = 8, .b = 7, .c = 2},
    { .a = 8, .b = 5, .c = 1},
    // top
    { .a = 2, .b = 7, .c = 5},
    { .a = 2, .b = 5, .c = 3},
    // bottom
    { .a = 6, .b = 8, .c = 1},
    { .a = 6, .b = 1, .c = 4},
};
```

**Lines**
Line drawing algorithms (Rasterize):
- **Naive algorithm**
- **Digital Differential Analyzer** (DDA graphics algorithm) — Similar to the naive line-drawing algorithm, with minor variations.
    - Warning: a bit slower because of the `/` operation
- **Bresenham's line algorithm** - Optimized to use only additions (i.e. no division Multiplications); it also avoids floating-point computations.
- **The Gupta-Sproull algorithm** - Based on Bresenham's line algorithm but adds antialiasing.
 