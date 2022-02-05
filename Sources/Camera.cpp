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
    glm::vec3  origin = this->getPosition();
    float angleX = (- 1 + 2.0f * x) * m_fov;
    float angleY = (- 1 + 2.0f * y) * m_fov;

    glm::vec3 directionInCamera = glm::normalize( glm::vec3(std::tanf(3.1415f * angleX / 180.0f), std::tanf(3.1415f * angleY / 180.0f), 1.0f) );

    return Ray(origin, directionInCamera);
}