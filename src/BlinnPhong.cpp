#include "BlinnPhong.h"
#include "Scene.h"

BlinnPhong::BlinnPhong(const color& diffuseColor, float exponent)
    : diffuseColor(diffuseColor), phongExp(exponent) {}

BlinnPhong::BlinnPhong(const color& diffuseColor, const color& specularColor, float exponent)
    : diffuseColor(diffuseColor), specularColor(specularColor), phongExp(exponent) {}


color BlinnPhong::rayColor(const Scene& world, const hit_record& hit, int depth) const
{
    color finalColor(0.0, 0.0, 0.0);
    Lambertian lambert(diffuseColor);
    color diffuse = lambert.rayColor(world, hit, depth);
    
    for (const auto& lightPtr : world.getLights()) {
        const auto& light = *lightPtr; // dereference
        
        if (!world.shadowChecker(hit, 0.001, 1.0, light)) {
            vec3 lightDir = unit_vector(light.getPosition() - hit.p);
            vec3 viewDir = unit_vector(-hit.r.direction());
            vec3 halfVec = unit_vector(lightDir + viewDir);
            float nDotH = std::max(0.0f, float(dot(hit.normal, halfVec)));
            float specFactor = std::pow(nDotH, phongExp);
            vec3 specular = specularColor * specFactor * light.getColor();
            finalColor += specular;
        }
    }
    finalColor += diffuse;
    return finalColor;
}