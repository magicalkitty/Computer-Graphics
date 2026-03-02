#include "MirrorShader.h"
#include "Scene.h"

color MirrorShader::rayColor(const Scene& world, const hit_record& hit, const Light& light, int depth) const
{
    vec3 incomingDir = unit_vector(hit.r.direction());
    vec3 reflectedDir = incomingDir-2.0f*dot(incomingDir, hit.normal) * hit.normal;
    ray reflectedRay(hit.p, reflectedDir);
    return world.computeRayColor(reflectedRay, 0.001, INFINITY, light, depth);
}