#include "RayTracer.h"

#include "Scene.h"
#include "hittable.h"
#include "NormalShader.h"
#include "Light.h"
#include "Shader.h"
#include <limits>

RayTracer::RayTracer(const Scene& world_, const color& background) : world(world_), backgroundColor(background) {}

color RayTracer::computeRayColor(const ray& r, float tmin, float& tmax, const Light& light, int depth) const
{
    if (depth <= 0) {
        return backgroundColor;
    }
    
    hit_record rec;
    float localTmax = tmax;
    bool hitShape = false;

    // find the closest hit

    if (world.intersect(r, tmin, localTmax, rec)) {
        // if the object has a shader, use it
        auto shader = rec.shaderPointer
            ? rec.shaderPointer
            : std::make_shared<NormalShader>();

        color c = shader->rayColor(world, rec, light, depth - 1);
        return c;
    
    }
    else {
        // return backgroundColor;
        return backgroundColor;
        // vec3 unit_direction = unit_vector(r.direction());
        // auto a = 0.5*(unit_direction.y() + 1.0);
        // return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }
}

bool RayTracer::shadowChecker(const hit_record& rec, float tmin, float& tmax, const Light& light) const
{
    vec3 lightingRay = light.getPosition() - rec.p;
        ray shadowRay(rec.p, lightingRay);

        hit_record tempRec;

        return world.intersect(shadowRay, tmin, tmax, tempRec);
        // for (int idx=0; idx<objects.size(); ++idx) {
        //     if (objects[idx]->intersect(shadowRay, tmin, tmax, tempRec)) {
        //         return true; // blocked --> shadow!
        //     }
        // }

        // return false; // visible --> no shadow!
}