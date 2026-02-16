#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include <memory>
#include "Shader.h"

using color = vec3;

class Shader;

class hit_record {
    public:
        point3 p = point3(0,0,0);
        vec3 normal = vec3(0,0,0);
        double t = -1.0; // to indicate no hit
        bool front_face = false;

        std::shared_ptr<Shader> shaderPointer;

        // for triangle intersection
        double alpha, beta, gamma;

        void set_face_normal(const ray& r, const vec3& outward_normal) {
            // Sets the hit record normal vector.
            // NOTE: the parameter outward_normal is assumed to have unit length

            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }

};

class hittable {
    public:
        virtual ~hittable() = default;

        std::shared_ptr<Shader> shader;

        virtual bool intersect(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
};

#endif // HITTABLE_H