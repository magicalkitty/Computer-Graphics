#pragma once

#include "Camera.h"

class PerspectiveCamera : public Camera{
    public:
    PerspectiveCamera(int nx, int ny, float fov);

    PerspectiveCamera(vec3 position, int nx, int ny, float fov);
    // PerspectiveCamera(vec3 origin, vec3 viewdir, float focallenght,...)
    // dir, focal, dimension, 

    ray generateRay(float i, float j, ray &r0) override;

    private:
        float l, r, b, t; //image plane coords
};