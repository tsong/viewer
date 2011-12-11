#include "camera.h"
#include <math.h>

#include <QDebug>

Camera::Camera() {
    radial = 2;
    azimuth = 0;
    zenith = 90;

    Vector3f origin;
    origin.x = 0;
    origin.y = 0;
    origin.z = 0;
    origin.w = 0;

    minRadial = DEFAULT_MIN_RADIAL;
    maxRadial = DEFAULT_MAX_RADIAL;
}

Camera::Camera(int radial, int azimuth, int zenith) {
    this->radial = radial;
    this->azimuth = azimuth;
    this->zenith = zenith;
}

Vector3f Camera::toCartesian() {
    Vector3f p;
    p.z = radial*sin(RAD(zenith))*cos(RAD(azimuth));
    p.x = radial*sin(RAD(zenith))*sin(RAD(azimuth));
    p.y = radial*cos(RAD(zenith));

    return p;
}

//getters
float Camera::getRadial() { return radial; }
float Camera::getAzimuth() { return azimuth; }
float Camera::getZenith() { return zenith; }
Vector3f Camera::getOrigin() { return origin; }
float Camera::getMinRadial() { return minRadial; }
float Camera::getMaxRadial() { return maxRadial; }

//setters
void Camera::setRadial(float r) {
    if (r <= minRadial) r = minRadial;
    if (r >= maxRadial) r = maxRadial;
    this->radial = r;
}

void Camera::setAzimuth(float azimuth) { this->azimuth = azimuth; }

void Camera::setZenith(float zenith) {
    if (zenith <= 0) zenith = 0.01;
    if (zenith >= 180) zenith = 179.99;
    this->zenith = zenith;
}

void Camera::setOrigin(Vector3f p) { origin = p; }

void Camera::setRadialBounds(float min, float max) {
    minRadial = min;
    maxRadial = max;

    if (minRadial > maxRadial) minRadial = maxRadial;
}
