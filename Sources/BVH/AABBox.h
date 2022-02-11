#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>
#include <vector>



class AABBox {

public:
	AABBox() {};
	AABBox(glm::vec3 cornerUp_, glm::vec3 cornerDown_) : cornerUp(cornerUp_), cornerDown(cornerDown_) {};
    inline void add(size_t triangle_index) { triangle_indeces.push_back(triangle_index); };

	glm::vec3 cornerUp;
    glm::vec3 cornerDown;
    std::vector<size_t> triangle_indeces;
};
