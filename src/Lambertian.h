#pragma once

#include "Shader.h"
#include "Light.h"
#include "../renderlib/vec3.h"
#include <algorithm>

using color = vec3;

class Lambertian : public Shader {
    public:
        Lambertian(const color& diffuseColor) : diffuseColor(diffuseColor) {}
        // Lambertian(const Light& lightDir) : lightDir(lightDir) {}; 
        // dot product between normal vector of object and light direction vector
        // if negative, then the light is behind the surface and we return 0: max(0, dot(hit.normal, light_direction))
        
        color rayColor(const Scene& world, const hit_record& hit, const Light& light, int depth) const override {

            if (world.shadowChecker(hit, 0.001, 1.0, light)) {
                return color(0.0, 0.0, 0.0);
            }
            
            vec3 lightDir = unit_vector(light.getPosition() - hit.p);
            float nDotl = std::max(0.0f, float(dot(hit.normal, lightDir)));
            vec3 lambertShade(nDotl, nDotl, nDotl);
            return lambertShade * diffuseColor;
        }
     private:
        color diffuseColor;   
};