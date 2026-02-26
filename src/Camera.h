#pragma once

#include <vector>
#include "../renderlib/vec3.h"
#include "../src/ray.h"

class Camera {
    public:
    Camera() : position(0, 0, 0), U(1, 0, 0), V(0, 1, 0), W(0, 0, 1),
    focallength(1.0f), imageplane_height(0.5f), imageplane_width(0.5f), nx(100), ny(100) {}

    Camera(int pixel_nx, int pixel_ny)
        : position(0, 0, 0), U(1, 0, 0), V(0, 1, 0), W(0, 0, 1),
            focallength(1.0f),
            nx(pixel_nx), ny(pixel_ny) {

                imageplane_width= 0.5f;
                float aspectratio = (float)nx/(float)ny;
                // using w / h set heifht of imageplane
                imageplane_height = imageplane_width / aspectratio;
            }

    Camera(vec3 position, int pixel_nx, int pixel_ny)
        : position(position), U(1, 0, 0), V(0, 1, 0), W(0, 0, 1),
            focallength(1.0f),
            nx(pixel_nx), ny(pixel_ny) {

                imageplane_width= 0.5f;
                float aspectratio = (float)nx/(float)ny;
                // using w / h set heifht of imageplane
                imageplane_height = imageplane_width / aspectratio;
            }

    virtual ~Camera() {}

    // camera needs to know pixel image dimensions
    virtual void generateRay(float i, float j, ray &r) = 0;

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

