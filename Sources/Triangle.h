#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>


class Triangle {
public:
	Triangle(glm::vec3 p0_, glm::vec3 p1_, glm::vec3 p2_) : p0(p0_), p1(p1_), p2(p2_) {};

	glm::vec3 p0;
	glm::vec3 p1;
	glm::vec3 p2;
};
