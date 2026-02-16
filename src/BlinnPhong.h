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