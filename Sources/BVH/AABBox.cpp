#include "AABBox.h"


bool AABBox::intersect(Ray& ray, float& tmin_) {
    // Slab Method
    // This is a branchless method and is known to be the fastest
    float tx1 = (cornerDown.x - ray.origin.x) * ray.inv_dir.x;
    float tx2 = (cornerUp.x   - ray.origin.x) * ray.inv_dir.x;

    float tmin = std::min(tx1, tx2);
    float tmax = std::max(tx1, tx2);

    float ty1 = (cornerDown.y - ray.origin.y) * ray.inv_dir.y;
    float ty2 = (cornerUp.y   - ray.origin.y) * ray.inv_dir.y;

    tmin = std::max(tmin, std::min(ty1, ty2));
    tmax = std::min(tmax, std::max(ty1, ty2));

    float tz1 = (cornerDown.z - ray.origin.z) * ray.inv_dir.z;
    float tz2 = (cornerUp.z   - ray.origin.z) * ray.inv_dir.z;

    tmin = std::max(tmin, std::min(tz1, tz2));
    tmax = std::min(tmax, std::max(tz1, tz2));

    if (tmax >= tmin) {
        // There is an intesection so fill the references
        tmin_ = tmin;
        return true;
    }

    return false;
}