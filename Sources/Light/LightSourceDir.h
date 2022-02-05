#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>

/// Basic camera model
class LightSourceDir {
public:
	LightSourceDir();
	LightSourceDir(glm::vec3 direction_, glm::vec3 color_, float intensity_);

	glm::vec3 direction;
	float intensity;
	glm::vec3 color;
};
