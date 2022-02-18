#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>
#include <vector>
#include <limits>
#include <algorithm>
#include <set>

#include "AABBox.h"
#include "../Ray.h"
#include "../Scene.h"


class BVH {

public:
    BVH() {};
    void init(const std::shared_ptr<Scene> scenePtr, bool debug = false);
    void init(const std::shared_ptr<Scene> scenePtr, std::vector<std::pair<size_t, size_t>>& triangles, bool debug = false, size_t depth = 0);
    ~BVH();

    bool intersect(const std::shared_ptr<Scene> scenePtr, RayHit& rayHit, Ray& ray, size_t& mesh_index, size_t& triangle_index);
    bool intersect(const std::shared_ptr<Scene> scenePtr, RayHit& rayHit, Ray& ray, size_t& mesh_index, size_t& triangle_index, float tmin);
    bool fastIntersect(const std::shared_ptr<Scene> scenePtr, Ray& ray);
    bool fastIntersect(const std::shared_ptr<Scene> scenePtr, Ray& ray, float tmin);
	

    const std::shared_ptr<Scene> scenePtr;
    AABBox box;
    size_t numOfVertex = 0;
    BVH* child_left = nullptr;
    BVH* child_right = nullptr;
    int axis = -1; // 0 for x, 1 for y and z for 2
    float median;
};
