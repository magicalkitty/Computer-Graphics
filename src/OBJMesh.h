#pragma once
#include <vector>
#include "model_obj.h"
#include "BoundingBox.h"
#include "BoundingVolumeNode.h"
#include "hittable.h"
#include "Triangle.h"
#include "Shader.h"
#include "BlinnPhong.h"


class OBJMesh : public hittable {
public:
    OBJMesh(const std::string& filename, Shader* defaultShader);
    ~OBJMesh();

    bool intersect(const ray& r, float ray_tmin, float ray_tmax, hit_record& rec) const override;
    BoundingBox boundingBox() const override;

    const std::vector<float>& getVertexBuffer() const { return m_vertexBuffer; }

private:
    BoundingVolumeNode* m_modelRoot;
    BoundingBox m_bbox;

    std::vector<float> m_vertexBuffer;
};