#include "Scene.h"
#include "NormalShader.h"

Scene::Scene() {}

Scene::Scene(shared_ptr<hittable> object) { add(object); }

Scene::Scene(shared_ptr<Light> light) { addLight(light); }

void Scene::add(shared_ptr<hittable> object) {
    objects.push_back(object);
}

void Scene::addLight(shared_ptr<Light> light) {
    lights.push_back(light);
}

bool Scene::intersect(const ray& r, float ray_tmin, float ray_tmax, hit_record& rec) const
{
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
}

bool Scene::shadowChecker(const hit_record& rec, float tmin, float tmax, const Light& light) const
{
    vec3 lightingRay = light.getPosition() - rec.p;
    ray shadowRay(rec.p, lightingRay);
    
    hit_record tempRec;
    
    for (int idx=0; idx<objects.size(); ++idx) {
        if (objects[idx]->intersect(shadowRay, tmin, tmax, tempRec)) {
            return true; // blocked --> shadow!
        }
    }

    return false; // visible --> no shadow!
}

color Scene::computeRayColor(const ray& r, float tmin, float tmax, int depth) const
{
    if (depth <= 0) {
        return backgroundColor;
    }
    
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
        auto shader = rec.shaderPointer
            ? rec.shaderPointer
            : std::make_shared<NormalShader>();

        color c = shader->rayColor(*this, rec, depth - 1);
        return c;
    
    }
    else {
        // return backgroundColor;
        return backgroundColor;
        // vec3 unit_direction = unit_vector(r.direction());
        // auto a = 0.5*(unit_direction.y() + 1.0);
        // return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }
}