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

        color rayColor(const Scene& world, const hit_record& hit, const Light& light, int depth) const override {
            if (world.shadowChecker(hit, 0.001, 1.0, light)) {
                return color(0.0, 0.0, 0.0);
            }
            
            Lambertian lambert(diffuseColor);
            color diffuse = lambert.rayColor(world, hit, light, depth);

            vec3 lightDir = unit_vector(light.getPosition() - hit.p);
            vec3 viewDir = unit_vector(-hit.r.direction());
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