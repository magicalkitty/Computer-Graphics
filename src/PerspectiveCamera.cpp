#include "PerspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera() : Camera() 
{
    focallength = 1.0f;

    l = -imageplane_width/2.0f; 
    r = imageplane_width/2.0f; 
    b = -imageplane_height/2.0f;
    t = imageplane_height/2.0f; 
}

PerspectiveCamera::PerspectiveCamera(int nx, int ny, float fov)
    : Camera(nx, ny)
{
    focallength = fov;

    l = -imageplane_width/2.0f; 
    r = imageplane_width/2.0f; 
    b = -imageplane_height/2.0f;
    t = imageplane_height/2.0f; 
}

PerspectiveCamera::PerspectiveCamera(vec3 position, int nx, int ny, float fov)
    : Camera(position, nx, ny)
    {
        focallength = fov;

        l = -imageplane_width/2.0f; 
        r = imageplane_width/2.0f; 
        b = -imageplane_height/2.0f;
        t = imageplane_height/2.0f; 
    }

PerspectiveCamera::PerspectiveCamera(vec3 position, vec3 viewDir, int nx, int ny, float fov)
    : Camera(position, viewDir, vec3(0, 1, 0), nx, ny)
{
    focallength = fov;

    l = -imageplane_width/2.0f; 
    r = imageplane_width/2.0f; 
    b = -imageplane_height/2.0f;
    t = imageplane_height/2.0f;
}

ray PerspectiveCamera::generateRay(float i, float j, ray &r0) 
{
    float u = l + (r-l)*(i+0.5f)/float(nx);
    float v = b + (t-b)*(j+0.5f)/float(ny);
    vec3 dir = -focallength*W + u*U + v*V;
    r0 = ray(position, dir);

    return r0;
}