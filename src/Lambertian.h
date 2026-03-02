#pragma once

#include "Shader.h"
#include "Light.h"
#include "../renderlib/vec3.h"
#include <algorithm>

using color = vec3;

class Lambertian : public Shader {
    public:
        Lambertian(const color& diffuseColor);
        // Lambertian(const Light& lightDir) : lightDir(lightDir) {}; 
        // dot product between normal vector of object and light direction vector
        // if negative, then the light is behind the surface and we return 0: max(0, dot(hit.normal, light_direction))
        
        color rayColor(const Scene& world, const hit_record& hit, const Light& light, int depth) const override;
    private:
        color diffuseColor;   
};