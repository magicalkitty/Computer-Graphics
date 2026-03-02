#pragma once

#include "Camera.h"

class PerspectiveCamera : public Camera{
    public:
    PerspectiveCamera();

    PerspectiveCamera(int nx, int ny, float fov);

    PerspectiveCamera(vec3 position, vec3 viewDir, int nx, int ny, float fov);

    ray generateRay(float i, float j, ray &r0) override;

    private:
        float l, r, b, t; //image plane coords
};