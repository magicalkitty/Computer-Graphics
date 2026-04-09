#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "../renderlib/vec3.h"
#include "Shader.h"
#include <memory>

using color = vec3;

class Sphere : public hittable {
    public:
        Sphere();
        Sphere(const point3& center, double radius, std::shared_ptr<Shader> shaderPtr = nullptr);

        const point3& get_center() const { return center; }
        double get_radius() const { return radius; }
        std::shared_ptr<Shader> getShader() const { return shader; }

        bool intersect(const ray& r, float ray_tmin, float ray_tmax, hit_record& rec) const override;

        BoundingBox boundingBox() const override {
            return BoundingBox(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
}
        
    private:
        point3 center;
        double radius;
        std::shared_ptr<Shader> shader;
};

#endif // SPHERE_H