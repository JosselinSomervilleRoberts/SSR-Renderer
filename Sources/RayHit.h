#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>

#include "Triangle.h"


class RayHit {
public:
	RayHit(Triangle& triangle_, float b0_, float b1_, float b2_, float t_) : triangle(triangle_), b0(b0_), b1(b1_), b2(b2_), t(t_) {};
    inline glm::vec3 hitPosition() { return b0*triangle.p0 + b1*triangle.p1 + b2*triangle.p2; };

	Triangle triangle;
	float b0;
    float b1;
    float b2;
    float t;
};
