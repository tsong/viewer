#include "light.h"

Light::Light() {
    ambient.r = 0;
    ambient.g = 0;
    ambient.b = 0;

    diffuse.r = 0;
    diffuse.g = 0;
    diffuse.b = 0;

    specular.r = 0;
    specular.g = 0;
    specular.b = 0;

    pos.x = 0;
    pos.y = 0;
    pos.z = 0;

    cutoff = 0;

    isEnabled = false;
}

Light::Light(Color a, Color d, Color s, Vector3f p) {
    ambient = a;
    diffuse = d;
    specular = s;
    pos = p;

    isEnabled = true;
}

Light::Light(float ar, float ag, float ab, float aa, float dr, float dg, float db,
             float da, float sr, float sg, float sb, float sa, float x, float y, float z)
{
    ambient.r = ar;
    ambient.g = ag;
    ambient.b = ab;
    ambient.a = aa;

    diffuse.r = dr;
    diffuse.g = dg;
    diffuse.b = db;
    diffuse.a = da;

    specular.r = sr;
    specular.g = sg;
    specular.b = sb;
    specular.a = sa;

    pos.x = x;
    pos.y = y;
    pos.z = z;
    pos.w = 1;

    isEnabled = true;
}
