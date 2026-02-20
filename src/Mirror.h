#pragma once

#include "Shader.h"
#include "Light.h"
#include "../renderlib/vec3.h"
#include <algorithm>

using color = vec3;

class Mirror : public Shader {
    public:
        color rayColor(const Scene& world, const hit_record& hit, const Light& light, int depth) const override {
            vec3 incomingDir = unit_vector(hit.r.direction());
            vec3 reflectedDir = incomingDir-2.0f*dot(incomingDir, hit.normal) * hit.normal;
            ray reflectedRay(hit.p, reflectedDir);
            return world.computeRayColor(reflectedRay, 0.001, INFINITY, light, depth);
        }
     private: 
};