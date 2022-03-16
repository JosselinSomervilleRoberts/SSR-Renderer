// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#include "BoundingBox.h"

#include <algorithm>

using namespace std;

size_t BoundingBox::dominantAxis () const {
    if (m_max.x - m_min.x > m_max.y - m_min.y && m_max.x - m_min.x > m_max.z - m_min.z)
        return 0;
    else if (m_max.y - m_min.y > m_max.x - m_min.x && m_max.y - m_min.y > m_max.z - m_min.z)
        return 1;
    return 2;
}

bool BoundingBox::contains (const glm::vec3 & p) const {
    for (size_t i = 0; i < 3; i++)
        if (!(p[i] >= m_min[i] && p[i] <= m_max[i]))
            return false;
    return true;
}

void BoundingBox::extendTo (const glm::vec3 & p) {
    for (size_t i = 0; i < 3; i++) {
        if (p[i] > m_max[i])
            m_max[i] = p[i];
        if (p[i] < m_min[i])
            m_min[i] = p[i];
    }
}

void BoundingBox::extendTo (const BoundingBox & b) {
    extendTo (b.m_min);
    extendTo (b.m_max);
}

void BoundingBox::translate (const glm::vec3 & t) {
    m_min += t;
    m_max += t;
}

void BoundingBox::scale (float factor) {
    glm::vec3 c = center ();
    m_min = c + factor * (m_min - c);
    m_max = c + factor * (m_max - c);
}

bool BoundingBox::intersect (const BoundingBox & b) const {
    for (size_t i = 0; i < 3; i++)
        if (m_max[i] < b.m_min[i] || m_min[i] > b.m_max[i])
            return false;
    return true;
}

