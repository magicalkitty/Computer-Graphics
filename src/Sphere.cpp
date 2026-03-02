#include "Sphere.h"

Sphere::Sphere() : center(point3(0,0,0)), radius(1.0), shader(nullptr) {}

Sphere::Sphere(const point3& center, double radius, std::shared_ptr<Shader> shaderPtr)
    : center(center), radius(radius), shader(shaderPtr) {};

bool Sphere::intersect(const ray& r, float ray_tmin, float ray_tmax, hit_record& rec) const
{
    vec3 oc = r.origin() - center;
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0f * dot(r.direction(), oc);
    auto c = dot(oc, oc) - (radius*radius);

    auto discriminant = b*b - 4*a*c;
    if (discriminant < 0) {
        return false;
    }

    auto sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-b-sqrtd) / (2*a);
    if (root <= ray_tmin || ray_tmax <= root) {
        root = (-b+sqrtd) / (2*a);
        if (root <= ray_tmin || ray_tmax <= root)
            return false;
    }


    if (ray_tmin < root && root < ray_tmax) {
        rec.t = root;
        rec.p = r.at(rec.t);
        rec.normal = unit_vector(rec.p - center);
        rec.set_face_normal(r, rec.normal);
        rec.shaderPointer = shader;
        rec.r = r;
        rec.shape = this;
        ray_tmax = rec.t;

        return true;
    }
    return false;
}
