#pragma once

#include "Shader.h"
#include "Light.h"
#include "../renderlib/vec3.h"
#include <algorithm>

using color = vec3;

class MirrorShader : public Shader {
    public:
        color rayColor(const Scene& world, const hit_record& hit, const Light& light, int depth) const override;
     private: 
};