#include "NormalShader.h"
#include "Scene.h"
#include <algorithm>

color NormalShader::rayColor(const Scene& world, const hit_record& hit, const Light& light, int depth) const {
    if (world.shadowChecker(hit, 0.001, 1.0, light)) {
        return color(0.0, 0.0, 0.0);
    }
    
    return 0.5 * (hit.normal + color(1.0, 1.0, 1.0));
}