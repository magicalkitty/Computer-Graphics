#pragma once

#include <vector>
#include "../renderlib/vec3.h"
#include "../src/ray.h"

class Camera {
    public:
    Camera() : position(0, 0, 0), U(1, 0, 0), V(0, 1, 0), W(0, 0, 1),
    focallength(1.0), imageplane_height(0.25), imageplane_width(0.25) {}
    virtual ~Camera() {}

    virtual void generateRay(int i, int j, ray &r) = 0;

    protected:
        point3 position; //vec3 or point3?

        // basis vectors for my camera
        vec3 U, V, W;

        float focallength; //also sometimes "d"

        float imageplane_width, imageplane_height;
};