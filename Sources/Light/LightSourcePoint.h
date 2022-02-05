#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>

/// Basic camera model
class LightSourcePoint {
public:
	LightSourcePoint();
	LightSourcePoint(glm::vec3 position_, glm::vec3 color_, float intensity_);

	glm::vec3 position;
	float intensity;
	glm::vec3 color;

    // Diffusion constants
    float a_c;
    float a_l;
    float a_q;
};
