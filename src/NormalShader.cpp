#include "NormalShader.h"
#include "Scene.h"
#include <algorithm>

color NormalShader::rayColor(const Scene& world, const hit_record& hit, const Light& light, int depth) const {
    
    return 0.5 * (hit.normal + color(1.0, 1.0, 1.0));
}