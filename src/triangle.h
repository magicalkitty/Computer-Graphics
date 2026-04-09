#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "../renderlib/vec3.h"

class Triangle : public hittable {
    public:
        Triangle();
        Triangle(const point3& a, const point3& b, const point3& c, std::shared_ptr<Shader> shaderPtr = nullptr);

        std::shared_ptr<Shader> getShader() const { return shader; }

        bool intersect(const ray& r, float ray_tmin, float ray_tmax, hit_record& rec) const override;
        BoundingBox boundingBox() const override;

    private:
        point3 a, b, c;
        std::shared_ptr<Shader> shader;
};

#endif // TRIANGLE_H