#pragma once

#include "Camera.h"

class PerspectiveCamera : public Camera{
    public:

    PerspectiveCamera() : Camera(200, 200) {
        focallength = 0.05f;

        l = -imageplane_width/2.0f; 
        r = imageplane_width/2.0f; 
        b = -imageplane_height/2.0f;
        t = imageplane_height/2.0f; 
    }
    // PerspectiveCamera(vec3 origin, vec3 viewdir, float focallenght,...)
    // dir, focal, dimension, 

    void generateRay(int i, int j, ray &r0) override 
    {
        float u, v; 

        u = l + (r-l)*(i+0.5f)/float(nx);
        v = b + (t-b)*(j+0.5f)/float(ny);
        vec3 dir = -focallength*W + u*U + v*V;
        r0 = ray(position, dir);

    };

    private:
        float l, r, b, t; //image plane coords
};