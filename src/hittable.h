#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

class hit_record {
    public:
        point3 p = point3(0,0,0);
        vec3 normal = vec3(0,0,0);
        double t = -1.0; // to indicate no hit
};

class hittable {
    public:
        virtual ~hittable() = default;

        virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
};

#endif // HITTABLE_H