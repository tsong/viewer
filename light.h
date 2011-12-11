#ifndef LIGHT_H
#define LIGHT_H

#include "types.h"

class Light {
    public:
        Light();
        Light(Color a, Color d, Color s, Vector3f p);
        Light(float ar, float ag, float ab, float aa, float dr, float dg, float db, float da,
              float sr, float sg, float sb, float sa, float x, float y, float z);

        Color ambient;
        Color diffuse;
        Color specular;
        Vector3f pos;

        float cutoff;

        bool isEnabled;
        bool isDirectional;
};

#endif // LIGHT_H
