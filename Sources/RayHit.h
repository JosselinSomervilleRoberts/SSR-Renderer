#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>


class RayHit {
public:
	RayHit(float b0_, float b1_, float b2_, float t_) : b0(b0_), b1(b1_), b2(b2_), t(t_) {};
    inline glm::vec3 hitPosition(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2) { return b0*p0 + b1*p1 + b2*p2; };

	float b0;
    float b1;
    float b2;
    float t;
};
