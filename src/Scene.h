#ifndef SCENE_H
#define SCENE_H

#include "hittable.h"
#include "Shader.h"
#include "Light.h"
#include "../renderlib/vec3.h"
#include "Framebuffer.h"

#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr;

class Scene : public hittable {
  public:
    std::vector<shared_ptr<hittable>> objects;
    vec3 backgroundColor = vec3(1.0, 1.0, 1.0);

    Scene() {}
    Scene(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<hittable> object) {
        objects.push_back(object);
    }

    bool intersect(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_tmax;

        for (const auto& object : objects) {
            if (object->intersect(r, ray_tmin, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    };

    color computeRayColor(const ray& r, double tmin, double tmax, Light& light) {
        hit_record rec;
        float localTmax = tmax;
        bool hitShape = false;

        // find the closest hit
        for (int idx=0; idx<objects.size(); ++idx) {
            if (objects[idx]->intersect(r, tmin, localTmax, rec)) {
                hitShape = true;
                localTmax = rec.t;
            }
        }

        if (hitShape) {
            // if the object has a shader, use it
            if (rec.shaderPointer) {
                std::shared_ptr<Shader> shader = rec.shaderPointer;
                color c = shader->rayColor(rec, light);
                return c;
            }
            else{
                return 0.5 * (rec.normal + color(1.0, 1.0, 1.0));
            }
        
        }
        else {
            // return backgroundColor;
            vec3 unit_direction = unit_vector(r.direction());
            auto a = 0.5*(unit_direction.y() + 1.0);
            return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
        }
    }
};
#endif
