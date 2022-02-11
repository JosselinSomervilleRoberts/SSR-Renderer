// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#include "Camera.h"


Ray Camera::rayAt (float x, float y) {
    glm::mat4 viewMat = inverse(computeViewMatrix());
    glm::vec3 viewRight = normalize(glm::vec3(viewMat[0]));
    glm::vec3 viewUp = normalize(glm::vec3(viewMat[1]));
    glm::vec3 viewDir = -normalize(glm::vec3(viewMat[2]));
    glm::vec3 eye = (glm::vec3(viewMat[3]));
    float w = 2.0f * float(tan(glm::radians(m_fov / 2.0f)));
    glm::vec3 rayDir = glm::normalize(viewDir + ((x - 0.5f) * m_aspectRatio * w) * viewRight + (((1.0f - y) - 0.5f) * w) * viewUp);
    return Ray(eye, rayDir);

    /*
    glm::vec3  origin = this->getPosition();
    float angleX = (- 1 + 2.0f * x) * m_fov;
    float angleY = (- 1 + 2.0f * y) * m_fov;

    glm::vec3 directionInCamera = glm::normalize( glm::vec3(std::tanf(3.1415f * angleX / 180.0f), std::tanf(3.1415f * angleY / 180.0f), 1.0f) );

    return Ray(origin, directionInCamera);*/
}