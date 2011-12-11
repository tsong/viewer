#ifndef TYPES_H
#define TYPES_H

#include "utils/vector.h"
#include "utils/matrix.h"

#define PI 3.14159265
#define RAD(x) (x/180.0 * PI)
#define MAX(x,y) (x > y ? x : y)
#define MIN(x,y) (x < y ? x : y)

#define NUM_LIGHTS 8

typedef struct color {
    float r;
    float g;
    float b;
    float a;
} Color;

/*typedef struct vector {
    float x;
    float y;
    float z;
    float w;
} Vector;

typedef struct matrix {
    float M[16];
} Matrix;*/

/*typedef Vector Vector4f;
typedef Matrix Matrix4f;*/


#endif // TYPES_H
