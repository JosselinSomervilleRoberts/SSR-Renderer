#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>


class Ray {
public:
	Ray();
	Ray(glm::vec3 origin_, glm::vec3 direction_);

	glm::vec3 origin;
	glm::vec3 direction;
};
