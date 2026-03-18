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
        BlinnPhong(const color& diffuseColor, float exponent = 32.0f);

        BlinnPhong(const color& diffuseColor, const color& specularColor, float exponent = 32.0f);

        color rayColor(const Scene& world, const hit_record& hit, int depth) const override;
    private:
        color diffuseColor;
        color specularColor = color(1.0, 1.0, 1.0);
        float phongExp;
};