#pragma once
#include "../renderlib/vec3.h"
#include "Ray.h"
#include <algorithm>
#include "Ray.h"

class BoundingBox {
public:
    BoundingBox() {
        // Initialize to an "empty" box
        for (int i=0;i<3;i++) {
            bmin[i] = 1e30;
            bmax[i] = -1e30;
        }
    }

    BoundingBox(const vec3& a, const vec3& b) {
        bmin = a;
        bmax = b;
    }

    const vec3& min() const { return bmin; }
    const vec3& max() const { return bmax; }

    void update(const vec3& p) {
        bmin.set_x(std::min(bmin.x(), p.x()));
        bmin.set_y(std::min(bmin.y(), p.y()));
        bmin.set_z(std::min(bmin.z(), p.z()));

        bmax.set_x(std::max(bmax.x(), p.x()));
        bmax.set_y(std::max(bmax.y(), p.y()));
        bmax.set_z(std::max(bmax.z(), p.z()));
    }

    bool hit(const ray& r, double t_min, double t_max) const {
        // Slab method
        for (int i=0;i<3;i++) {
            double invD = 1.0 / r.direction()[i];
            double t0 = (bmin[i] - r.origin()[i]) * invD;
            double t1 = (bmax[i] - r.origin()[i]) * invD;
            if (invD < 0.0) std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min) return false;
        }
        return true;
    }

private:
    vec3 bmin;
    vec3 bmax;
};

// Helper function to combine two bounding boxes
inline BoundingBox surroundingBox(const BoundingBox& box0, const BoundingBox& box1) {
    vec3 small(
        std::min(box0.min().x(), box1.min().x()),
        std::min(box0.min().y(), box1.min().y()),
        std::min(box0.min().z(), box1.min().z())
    );
    vec3 big(
        std::max(box0.max().x(), box1.max().x()),
        std::max(box0.max().y(), box1.max().y()),
        std::max(box0.max().z(), box1.max().z())
    );
    return BoundingBox(small, big);
}