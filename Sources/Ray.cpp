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


bool Ray::intersect(RayHit& rayHit, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2) const {
	float epsilon = 0.00001f;

	glm::vec3 e0 = p1 - p0;
	glm::vec3 e1 = p2 - p0;
	glm::vec3 n = glm::normalize(glm::cross(e0, e1));
	glm::vec3 q = glm::cross(this->direction, e1);
	float a = glm::dot(e0, q);

	if((glm::dot(n, this->direction) >= 0) || (std::fabs(a) < epsilon))
		return false;

	glm::vec3 s = (this->origin - p0) / a;
	glm::vec3 r = glm::cross(s, e0);

	float b0 = glm::dot(s, q);
	float b1 = glm::dot(r, this->direction);
	float b2 = 1 - b0 - b1;

	if ((b0 < 0) || (b1 < 0) || (b2 < 0))
		return false;

	float t = glm::dot(e1, r);

	if ((t >= 0) && (t < rayHit.t)) {
		rayHit.b0 = b0;
		rayHit.b1 = b1;
		rayHit.b2 = b2;
		rayHit.t = t;
		return true;
	}

	return false;
}