#include "Ray.h"


Ray::Ray(glm::vec3 origin_, glm::vec3 direction_) :
    origin(origin_)
{
    setDirection(direction_);
}

Ray::Ray() : Ray(glm::vec3(0., 0., 0.), glm::vec3(1., 0., 0.)) {}

void Ray::setDirection(glm::vec3 direction_) {
    direction = direction_;
    inv_dir.x = 1.0f / direction.x;
    inv_dir.y = 1.0f / direction.y;
    inv_dir.z = 1.0f / direction.z;
}

