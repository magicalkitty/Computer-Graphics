#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "../renderlib/vec3.h"
#include "Shader.h"
#include <memory>

using color = vec3;

class Sphere : public hittable {
    public:
        Sphere(const point3& center, double radius, std::shared_ptr<Shader> shaderPtr) : center(center), radius(std::fmax(0, radius)), shader(shaderPtr) {}

        const point3& get_center() const { return center; }
        double get_radius() const { return radius; }
        std::shared_ptr<Shader> getShader() const { return shader; }
        void setShader(std::shared_ptr<Shader> shaderPtr) {shader = shaderPtr;}


        bool intersect(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
            vec3 oc = center -r.origin();
            auto a = r.direction().length_squared();
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius*radius;

            auto discriminant = h*h - a*c;
            if (discriminant < 0) {
                return false;
            }

            auto sqrtd = std::sqrt(discriminant);

            // Find the nearest root that lies in the acceptable range.
            auto root = (h-sqrtd) / a;
            if (root <= ray_tmin || ray_tmax <= root) {
                root = (h+sqrtd) / a;
                if (root <= ray_tmin || ray_tmax <= root)
                    return false;
            }

            rec.t = root;
            rec.p = r.at(rec.t);
            rec.normal = unit_vector(rec.p - center);
            rec.set_face_normal(r, rec.normal);
            rec.shaderPointer = shader;

            return true;
        }
        
    private:
        point3 center;
        double radius;
        std::shared_ptr<Shader> shader;
};

#endif // SPHERE_H