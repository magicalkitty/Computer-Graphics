#include "Camera.h"

Camera::Camera()
    : position(0, 0, 0), U(1, 0, 0), V(0, 1, 0), W(0, 0, 1),
    focallength(1.0f), imageplane_height(0.5f), imageplane_width(0.5f), nx(100), ny(100) {}

Camera::Camera(int pixel_nx, int pixel_ny)
    : position(0, 0, 0), U(1, 0, 0), V(0, 1, 0), W(0, 0, 1),
    focallength(1.0f), nx(pixel_nx), ny(pixel_ny) 
{
    imageplane_width= 0.5f;
    float aspectratio = (float)nx/(float)ny;
    // using w / h set height of imageplane
    imageplane_height = imageplane_width / aspectratio;
}

Camera::Camera(vec3 position, int pixel_nx, int pixel_ny)
    : position(position), U(1, 0, 0), V(0, 1, 0), W(0, 0, 1),
    focallength(1.0f), nx(pixel_nx), ny(pixel_ny)
    {
        imageplane_width= 0.5f;
        float aspectratio = (float)nx/(float)ny;
        // using w / h set heifht of imageplane
        imageplane_height = imageplane_width / aspectratio;
    }

Camera::Camera(vec3 position, vec3 viewDir, vec3 upDir, int pixel_nx, int pixel_ny)
    : position(position), U(1, 0, 0), V(0, 1, 0), W(0, 0, 1),
    focallength(1.0f), nx(pixel_nx), ny(pixel_ny)
{
    imageplane_width= 0.5f;
    float aspectratio = (float)nx/(float)ny;
    // using w / h set height of imageplane
    imageplane_height = imageplane_width / aspectratio;

    W = -unit_vector(viewDir);
    
    vec3 t = upDir;

    // If the up direction is parallel to the view direction, we need to choose a different up vector to avoid a degenerate camera basis.
    if (std::abs(dot(unit_vector(t), W)) > 0.999f) {
        t = vec3(0, 0, 1); // Choose a different up vector
    }

    U = unit_vector(cross(t, W));
    V = cross(W, U);
}