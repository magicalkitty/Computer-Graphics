#pragma once

#include "Scene.h"
#include "hittable.h"
#include "Shader.h"
#include "Light.h"
#include "ray.h"
#include "../renderlib/vec3.h"
#include "Light.h"
#include <memory>
#include <vector>
using color = vec3;

class hittable;
class Light;

class RayTracer {
    public:
        RayTracer(const Scene& world, const color& backgroundColor = vec3(178.0/255.0, 221.0/255.0, 235.0/255.0)) : world(world), backgroundColor(backgroundColor) {};

        color computeRayColor(const ray& r, float tmin, float& tmax, const Light& light, int depth) const;

        bool shadowChecker(const hit_record& rec, float tmin, float& tmax, const Light& light) const;
        

    private:
        const Scene& world;
        color backgroundColor;
};

