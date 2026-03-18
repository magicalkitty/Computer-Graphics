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
    std::vector<shared_ptr<Light>> lights;
    vec3 backgroundColor = vec3(178.0/255.0, 221.0/255.0, 235.0/255.0);

    Scene();
    Scene(shared_ptr<hittable> object);
    Scene(shared_ptr<Light> light);

    const std::vector<std::shared_ptr<Light>>& getLights() const {
      return lights;
    }

    void clear() { objects.clear(); }

    void add(shared_ptr<hittable> object);

    void addLight(shared_ptr<Light> light);

    bool intersect(const ray& r, float ray_tmin, float ray_tmax, hit_record& rec) const override;

    bool shadowChecker(const hit_record& rec, float tmin, float tmax, const Light& light) const;

    color computeRayColor(const ray& r, float tmin, float tmax, int depth) const;
};
#endif // SCENE_H
