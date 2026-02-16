#pragma once

#include "Shader.h"
#include "Light.h"
#include "../renderlib/vec3.h"
#include <algorithm>
#include <cmath>
#include "Lambertian.h"

using color = vec3;

class BlinnPhong : public Shader {
    public:
        BlinnPhong(const color& diffuseColor, float exponent = 32.0f) : diffuseColor(diffuseColor), phongExp(exponent) {}

        // Lambertian(const Light& lightDir) : lightDir(lightDir) {}; 
        // dot product between normal vector of object and light direction vector
        // if negative, then the light is behind the surface and we return 0: max(0, dot(hit.normal, light_direction))
        
        color rayColor(const hit_record& hit, const Light& light) override {
            Lambertian lambert(diffuseColor);
            color diffuse = lambert.rayColor(hit, light);

            vec3 lightDir = unit_vector(light.getPosition() - hit.p);
            vec3 viewDir = unit_vector(-hit.p);
            vec3 halfVec = unit_vector(lightDir + viewDir);
            float nDotH = std::max(0.0f, float(dot(hit.normal, halfVec)));
            float specFactor = std::pow(nDotH, phongExp);
            vec3 specular(specFactor, specFactor, specFactor);

            return diffuse + specular * light.getColor();
        }
    private:
        color diffuseColor;
        float phongExp;
};