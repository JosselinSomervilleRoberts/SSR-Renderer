#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>
#include <vector>

#include "../Ray.h"


class AABBox {

public:
	AABBox() {};
	AABBox(glm::vec3 cornerUp_, glm::vec3 cornerDown_) : cornerUp(cornerUp_), cornerDown(cornerDown_) {};

    inline void add(size_t mesh_index, size_t triangle_index) { triangles.push_back(std::make_pair(mesh_index, triangle_index)); };
    inline void add(std::pair<size_t, size_t> triangle) { triangles.push_back(triangle); };

    bool intersect(Ray& ray, float& tmin_);

	glm::vec3 cornerUp;
    glm::vec3 cornerDown;
    std::vector<std::pair<size_t, size_t>> triangles;
};
