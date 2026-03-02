#pragma once
#include "Shader.h"
#include "../renderlib/vec3.h"
#include "Scene.h"
#include <memory>

using color = vec3;

class DiffuseShader : public Shader {
public:
    DiffuseShader(const color& diffuseColor);

    vec3 random_in_unit_sphere() const;

    color rayColor(const Scene& world, const hit_record& hit, const Light& light, int depth) const override;

private:
    color diffuseColor;

    // helper to get random unit vector in a hemisphere around the normal
    vec3 random_in_hemisphere(const vec3& normal) const;
};