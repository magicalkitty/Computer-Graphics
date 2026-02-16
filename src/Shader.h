#pragma once

#include "hittable.h"
#include "../renderlib/vec3.h"
#include "Ray.h"
#include "Light.h"
#include <memory>

struct hit_record;

class Shader {
    public:
        virtual ~Shader()  = default;
        virtual color rayColor( const hit_record& hit, const Light& light ) = 0;
};