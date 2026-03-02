#pragma once

#include "hittable.h"
#include "../renderlib/vec3.h"
#include "Ray.h"
#include "Light.h"
#include <memory>
using color = vec3;

struct hit_record;
class Scene;
class Light;

class Shader {
    public:
        virtual ~Shader()  = default;
        virtual color rayColor( const Scene& world, const hit_record& hit, const Light& light, int depth = 10) const = 0;
};