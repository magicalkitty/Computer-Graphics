#pragma once

#include "hittable.h"
#include "../renderlib/vec3.h"
#include "Ray.h"
#include "Light.h"
#include <memory>

using color = vec3;

struct hit_record;
class Light;

class Shader {
    public:
        virtual ~Shader()  = default;
        virtual color rayColor( const hit_record& hit, const Light& light ) = 0;
};