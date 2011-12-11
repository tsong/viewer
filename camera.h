#ifndef CAMERA_H
#define CAMERA_H

#include "types.h"

#define DEFAULT_MAX_RADIAL 5
#define DEFAULT_MIN_RADIAL 2

/* An arc-ball camera that stores its location in polar coordinates
   of a hemisphere and distance from origin*/
class Camera {
    public:
        Camera();
        Camera(int radial, int azimuth, int zenith);
        Vector3f toCartesian();

        //getters
        float getRadial();
        float getAzimuth();
        float getZenith();
        Vector3f getOrigin();
        float getMinRadial();
        float getMaxRadial();

        //setters
        void setRadial(float r);
        void setAzimuth(float azimuth);
        void setZenith(float zenith);
        void setOrigin(Vector3f p);
        void setRadialBounds(float min, float max);

    private:
        float radial;
        float azimuth;
        float zenith;

        Vector3f origin;
        float minRadial;
        float maxRadial;
};

#endif // CAMERA_H
