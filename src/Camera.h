#pragma once

#include <vector>
#include "../renderlib/vec3.h"
#include "ray.h"

class Camera {
    public:
    Camera();

    Camera(int pixel_nx, int pixel_ny);

    Camera(vec3 position, int pixel_nx, int pixel_ny);

    Camera(vec3 position, vec3 viewDir, vec3 upDir, int pixel_nx, int pixel_ny);

    virtual ~Camera() {}

    // camera needs to know pixel image dimensions
    virtual ray generateRay(float i, float j, ray &r) = 0;

    // Virtual Ray generateRay(int i, int j) = 0;

    // future refactor
    // virtual void generate(float i, float j, ray &r) = 0;
    // virtual ray generate(float i, float j) = 0;
    
    protected:
        vec3 position;

        // basis vectors for my camera
        vec3 U, V, W;

        float focallength; //also sometimes "d"

        float imageplane_width, imageplane_height;

        int nx, ny; //image resolution
};

