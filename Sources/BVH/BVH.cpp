#include "BVH.h"


float findMedian(std::vector<float> a, size_t n)
{
  
    // If size of the arr[] is even
    if (n % 2 == 0) {
  
        // Applying nth_element
        // on n/2th index
        std::nth_element(a.begin(),
                    a.begin() + n / 2,
                    a.end());
  
        // Applying nth_element
        // on (n-1)/2 th index
        std::nth_element(a.begin(),
                    a.begin() + (n - 1) / 2,
                    a.end());
  
        // Find the average of value at
        // index N/2 and (N-1)/2
        return (float)(a[(n - 1) / 2]
                        + a[n / 2])
               / 2.0f;
    }
  
    // If size of the arr[] is odd
    else {
  
        // Applying nth_element
        // on n/2
        nth_element(a.begin(),
                    a.begin() + n / 2,
                    a.end());
  
        // Value at index (N/2)th
        // is the median
        return (float)a[n / 2];
    }
}


void BVH::init(const std::shared_ptr<Scene> scenePtr, bool debug) 
{
    // This constructor should only be called for the root

    std::vector<std::pair<size_t, size_t>> triangles;
    size_t numOfMeshes = scenePtr->numOfMeshes ();
	for (size_t i = 0; i < numOfMeshes; i++) {
        const std::shared_ptr<Mesh>& mesh = scenePtr->mesh(i);
        const size_t nbTriangles  = mesh->triangleIndices().size();

        for(size_t k=0; k<nbTriangles; k++) {
            triangles.push_back(std::make_pair(i, k));
        } 
    }

    init(scenePtr, triangles, debug, 0);
}




void BVH::init(const std::shared_ptr<Scene> scenePtr, std::vector<std::pair<size_t, size_t>>& triangles, bool debug, size_t depth) {
    if (debug) {
        for(size_t i =0; i < 2*depth; i++) std::cout << " ";
        std::cout << "-> BVH node : ";
    }

    // 0. Copy the triangle indices
    for (size_t i = 0; i < triangles.size(); i++)
        box.add(triangles[i].first, triangles[i].second);


    // 1. Determine the size of the box
    float dimMin[3];
    float dimMax[3];
    for(int i=0; i<3; i++) {
        dimMin[i] = std::numeric_limits<float>::max();
        dimMax[i] = std::numeric_limits<float>::min();
    }

	for (size_t i = 0; i < triangles.size(); i++) {
        std::pair<size_t, size_t>& pair = box.triangles[i];
        const std::shared_ptr<Mesh>& mesh = scenePtr->mesh(pair.first);
        glm::uvec3& triangleIndex  = mesh->triangleIndices()[pair.second];
        const std::vector<glm::vec3>& vertexPositions  = mesh->vertexPositions();
        
        for(size_t k=0; k<3; k++) {
            for(int j=0; j<3; j++) {
                if (vertexPositions[triangleIndex[k]][j] < dimMin[j]) dimMin[j] = vertexPositions[triangleIndex[k]][j];
                if (vertexPositions[triangleIndex[k]][j] > dimMax[j]) dimMax[j] = vertexPositions[triangleIndex[k]][j];
            }
        }
    }
    box.cornerDown = glm::vec3(dimMin[0], dimMin[1], dimMin[2]);
    box.cornerUp   = glm::vec3(dimMax[0], dimMax[1], dimMax[2]);
    if (debug) std::cout << "(" << box.cornerDown[0] << ", " << box.cornerDown[1] << ", " << box.cornerDown[2] << ") - (" << box.cornerUp[0] << ", " << box.cornerUp[1] << ", " << box.cornerUp[2] << ")";


    // 1.5. SHORTCUT (BASE CASE)
    if(triangles.size() == 1) {
        if (debug) std::cout << " ONE TRIANGLE END" << std::endl;
        return;
    }
    else if(triangles.size() == 2) {
        if (debug) std::cout << " TWO TRIANGLE END" << std::endl;

        std::vector<std::pair<size_t, size_t>> triangles1;
        triangles1.push_back(std::make_pair(triangles[0].first, triangles[0].second));
        child_left = new BVH();
        child_left->init(scenePtr, triangles1, debug, depth + 1);
        
        std::vector<std::pair<size_t, size_t>> triangles2;
        triangles2.push_back(std::make_pair(triangles[1].first, triangles[1].second));
        child_right = new BVH();
        child_right->init(scenePtr, triangles2, debug, depth + 1);

        return;
    }

    // 2. Find the largest axis
    axis = 0;
    float size = dimMax[0] - dimMin[0];
    for(int i=1; i<3; i++) {
        float s = dimMax[i] - dimMin[i];
        if(s > size) {
            size = s;
            axis = i;
        }
    }
    if (debug) std::cout << " - Axis " << axis;



    // 3. Find the median of the axis

    // First we create a set with the vertex of the triangles
    // This is useful so that if a vertex is part of several triangles it is not added several times
    std::vector<float> pos;
    for (size_t i = 0; i < triangles.size(); i++) {
        std::pair<size_t, size_t>& pair = box.triangles[i];
        const std::shared_ptr<Mesh>& mesh = scenePtr->mesh(pair.first);
        glm::uvec3& triangleIndex  = mesh->triangleIndices()[pair.second];
        for(size_t k=0; k<3; k++) {
            pos.push_back(mesh->vertexPositions()[triangleIndex[k]][axis]);
        }
    }
    numOfVertex = pos.size();
    // And find the median
    median = findMedian(pos, numOfVertex);
    if (debug) std::cout << " - Median " << median;
    if (debug) std::cout << " - Triangles: " << triangles.size() << " - Vertex: " << numOfVertex << std::endl;


    // 4. Separate the triangles
    std::vector<std::pair<size_t, size_t>> trianglesRight;
    std::vector<std::pair<size_t, size_t>> trianglesLeft;

    for (size_t i = 0; i < triangles.size(); i++) {
        std::pair<size_t, size_t> pair = box.triangles[i];

        bool needToAdd = true;
        if(i+1 ==  triangles.size()) {
            if(trianglesRight.size() == 0){
                trianglesRight.push_back(pair);
                needToAdd = false;
            }
            if(trianglesLeft.size() == 0)  {
                trianglesLeft.push_back(pair);
                needToAdd = false;
            }
        }

        if(needToAdd) {
            const std::shared_ptr<Mesh>& mesh = scenePtr->mesh(pair.first);
            glm::uvec3& triangleIndex  = mesh->triangleIndices()[pair.second];
            const std::vector<glm::vec3>& vertexPositions  = mesh->vertexPositions();
            
            size_t right = 0;
            size_t equality = 0;
            for(size_t k=0; k<3; k++) {
                if (vertexPositions[triangleIndex[k]][axis] > median) right++;
                if (vertexPositions[triangleIndex[k]][axis] == median) equality++;
            }

            if(equality == 3) {
                if(trianglesRight.size() >= trianglesLeft.size()) trianglesLeft.push_back(pair);
                else trianglesRight.push_back(pair);
            }
            else if(equality == 2) {
                if(right > 0) trianglesRight.push_back(pair);
                else trianglesLeft.push_back(pair);
            }
            else if(equality == 1) {
                if(right == 2) trianglesRight.push_back(pair);
                else if(right == 0) trianglesLeft.push_back(pair);
                else {
                    if(trianglesRight.size() >= trianglesLeft.size()) trianglesLeft.push_back(pair);
                    else trianglesRight.push_back(pair);
                }
            }
            else if(right >= 2) trianglesRight.push_back(pair);
            else trianglesLeft.push_back(pair);
        }
    }

    // 5. Create the childs
    if(triangles.size() >= 2) { // if it's not a leaf
        child_left  = new BVH();
        child_left->init(scenePtr, trianglesLeft, debug, depth + 1);  // Has at least 1
        child_right = new BVH();
        child_right->init(scenePtr, trianglesRight, debug, depth + 1); // Has at least 1
    }
}

BVH::~BVH() {
    if(child_left != nullptr) delete child_left;
    if(child_right != nullptr) delete child_right;
}


bool BVH::intersect(const std::shared_ptr<Scene> scenePtr, RayHit& rayHit, Ray& ray, size_t& mesh_index, size_t& triangle_index, float tmin) {
    // To optimisize (so that we do not check useless boxes)
    if(tmin >= rayHit.t) // Thi means that we won't find a closer intersection
        return false;

    // If we have a leaf, then no need to check intersection with box, let's check
    // intersection with triangle to save time
    if(child_left == nullptr) { // If it's a leaf
        std::pair<size_t, size_t>& pair = box.triangles[0];
        const std::shared_ptr<Mesh>& mesh = scenePtr->mesh(pair.first);
        glm::uvec3& triangleIndex  = mesh->triangleIndices()[pair.second];
        glm::vec3& p0 = mesh->vertexPositions()[triangleIndex[0]];
        glm::vec3& p1 = mesh->vertexPositions()[triangleIndex[1]];
        glm::vec3& p2 = mesh->vertexPositions()[triangleIndex[2]];
        
        bool hit = ray.intersect(rayHit, p0, p1, p2);
        if(hit) {
            mesh_index = pair.first;
            triangle_index = pair.second;
            return true;
        }
        return false;
    }

    // We now check with childs
    float tminRight = 0;
    bool intersectRight = child_right->box.intersect(ray, tminRight);
    float tminLeft = 0;
    bool intersectLeft = child_left->box.intersect(ray, tminLeft);
    
    if(!intersectLeft && !intersectRight) return false;
    else if(!intersectRight) return child_left->intersect(scenePtr,  rayHit, ray, mesh_index, triangle_index, tminLeft);
    else if(!intersectLeft)  return child_right->intersect(scenePtr,  rayHit, ray, mesh_index, triangle_index, tminRight);
    else if(tminRight < tminLeft) {
        bool intesect_right = child_right->intersect(scenePtr, rayHit, ray, mesh_index, triangle_index, tminRight);
        bool intesect_left  = child_left->intersect(scenePtr,  rayHit, ray, mesh_index, triangle_index, tminLeft);
        return (intesect_left || intesect_right);
    }
    else {
        bool intesect_left  = child_left->intersect(scenePtr,  rayHit, ray, mesh_index, triangle_index, tminLeft);
        bool intesect_right = child_right->intersect(scenePtr, rayHit, ray, mesh_index, triangle_index, tminRight);
        return (intesect_left || intesect_right);
    }
}


bool BVH::intersect(const std::shared_ptr<Scene> scenePtr, RayHit& rayHit, Ray& ray, size_t& mesh_index, size_t& triangle_index) {
    float tmin = 0;
    bool hit = box.intersect(ray, tmin);
    if(!hit) return false;
    return this->intersect(scenePtr, rayHit, ray, mesh_index, triangle_index, tmin);
}