// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#include "Camera.h"


void Camera::computeVectorsForRayAt(glm::vec3& viewRight, glm::vec3& viewUp, glm::vec3& viewDir, glm::vec3& eye, float& w) {
    glm::mat4 viewMat = inverse(computeViewMatrix());
    viewRight = normalize(glm::vec3(viewMat[0]));
    viewUp = normalize(glm::vec3(viewMat[1]));
    viewDir = -normalize(glm::vec3(viewMat[2]));
    eye = (glm::vec3(viewMat[3]));
    w = 2.0f * float(tan(glm::radians(m_fov / 2.0f)));
}

Ray Camera::rayAt (float x, float y) {
    glm::vec3 viewRight,  viewUp,  viewDir,  eye;
    float w;
    computeVectorsForRayAt(viewRight, viewUp, viewDir, eye, w);
    
    return rayAt(x, y, viewRight, viewUp, viewDir, eye, w);
}

Ray Camera::rayAt (float x, float y, glm::vec3& viewRight, glm::vec3& viewUp, glm::vec3& viewDir, glm::vec3& eye, float& w) {
    glm::vec3 rayDir = glm::normalize(viewDir + ((x - 0.5f) * m_aspectRatio * w) * viewRight + (((1.0f - y) - 0.5f) * w) * viewUp);
    return Ray(eye, rayDir);
}