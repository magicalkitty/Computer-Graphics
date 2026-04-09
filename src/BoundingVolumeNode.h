#pragma once
#include "hittable.h"
#include "BoundingBox.h"
#include <vector>
#include <algorithm>
#include <random>

class BoundingVolumeNode : public hittable {
public:
    BoundingVolumeNode(std::vector<hittable*>& objects, double time);
    ~BoundingVolumeNode();

    bool intersect(const ray& r, float t_min, float t_max, hit_record& rec) const override;
    virtual BoundingBox boundingBox() const override;


private:
    hittable* m_left;
    hittable* m_right;
    BoundingBox m_bbox;
};