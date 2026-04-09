#include "BoundingVolumeNode.h"

// Comparator helpers — sort by centroid along a given axis
static bool boxCompare(hittable* a, hittable* b, int axis) {
    return a->boundingBox().min()[axis] < b->boundingBox().min()[axis];
}
static bool boxXCompare(hittable* a, hittable* b) { return boxCompare(a, b, 0); }
static bool boxYCompare(hittable* a, hittable* b) { return boxCompare(a, b, 1); }
static bool boxZCompare(hittable* a, hittable* b) { return boxCompare(a, b, 2); }

BoundingVolumeNode::BoundingVolumeNode(std::vector<hittable*>& objects, double time) {
    // Pick a random axis to split on
    static std::mt19937 rng(42);
    std::uniform_int_distribution<int> axisDist(0, 2);
    int axis = axisDist(rng);

    auto comparator = (axis == 0) ? boxXCompare
                    : (axis == 1) ? boxYCompare
                                  : boxZCompare;

    size_t n = objects.size();

    if (n == 1) {
        // Leaf: both children point to the same object to avoid null checks
        m_left = m_right = objects[0];
    }
    else if (n == 2) {
        // Sort and assign directly
        if (comparator(objects[0], objects[1])) {
            m_left  = objects[0];
            m_right = objects[1];
        } else {
            m_left  = objects[1];
            m_right = objects[0];
        }
    }
    else {
        // Sort the span and recurse on each half
        std::sort(objects.begin(), objects.end(), comparator);

        size_t mid = n / 2;
        std::vector<hittable*> leftList(objects.begin(), objects.begin() + mid);
        std::vector<hittable*> rightList(objects.begin() + mid, objects.end());

        m_left  = new BoundingVolumeNode(leftList,  time);
        m_right = new BoundingVolumeNode(rightList, time);
    }

    // Compute this node's bounding box from its two children
    m_bbox = surroundingBox(m_left->boundingBox(), m_right->boundingBox());
}

BoundingVolumeNode::~BoundingVolumeNode() {
    // Only delete children that are interior nodes we allocated;
    // leaf hittables (triangles, etc.) are owned by the caller.
    // If you want full ownership, track which pointers you allocated.
    if (m_left != m_right) {
        delete m_left;
        delete m_right;
    } else {
        delete m_left; // n==1 case, both point to same object
    }
}

bool BoundingVolumeNode::intersect(const ray& r, float t_min, float t_max, hit_record& rec) const {
    // Early-out: if ray misses this node's box, skip entirely
    if (!m_bbox.hit(r, t_min, t_max))
        return false;

    bool hitLeft  = m_left->intersect(r, t_min, t_max, rec);
    // If left was hit, tighten t_max so we only accept closer right hits
    bool hitRight = m_right->intersect(r, t_min, hitLeft ? rec.t : t_max, rec);

    return hitLeft || hitRight;
}

BoundingBox BoundingVolumeNode::boundingBox() const {
    return m_bbox;
}