#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>

#include "RayHit.h"


class Ray {

public:
	Ray();
	Ray(glm::vec3 origin_, glm::vec3 direction_);

	void setDirection(glm::vec3 direction_);
	glm::vec3& getDirection() { return direction; };

	bool intersect(RayHit& rayHit, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2) const;

	glm::vec3 origin;
	glm::vec3 direction;
	glm::vec3 inv_dir;
};
