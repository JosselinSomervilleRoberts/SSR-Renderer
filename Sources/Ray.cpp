#include "Ray.h"


Ray::Ray(glm::vec3 origin_, glm::vec3 direction_) :
    origin(origin_), direction(direction_)
{}

Ray::Ray() : Ray(glm::vec3(0., 0., 0.), glm::vec3(1., 0., 0.)) {}