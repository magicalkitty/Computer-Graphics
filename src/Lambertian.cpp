#include "Lambertian.h"
#include "Scene.h"

Lambertian::Lambertian(const color& diffuseColor)
    : diffuseColor(diffuseColor) {}

color Lambertian::rayColor(const Scene& world, const hit_record& hit, int depth) const 
{
    color finalColor(0.0, 0.0, 0.0);
    for (const auto& lightPtr : world.getLights())
    {
        const auto& light = *lightPtr; // dereference
        if (!world.shadowChecker(hit, 0.001, 1.0, light)) {
            vec3 lightDir = unit_vector(light.getPosition() - hit.p);
            float nDotl = std::max(0.0f, float(dot(hit.normal, lightDir)));
            vec3 lambertShade(nDotl, nDotl, nDotl);
            finalColor += (lambertShade * diffuseColor) * light.getColor();
        }
    }
        return finalColor;
}