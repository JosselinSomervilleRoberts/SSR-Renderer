// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#pragma once

#include <algorithm>

#include <glm/glm.hpp>
#include <glm/ext.hpp>


static const float BBOX_EPSILON (0.000001f);
        

class BoundingBox {
    public:
        inline BoundingBox () {}
        
        inline BoundingBox (const glm::vec3 & p) : m_min (p), m_max (p) {}
        
        inline BoundingBox (const glm::vec3 & minP, const glm::vec3 & maxP) : m_min (minP), m_max (maxP) {}
        
        inline virtual ~BoundingBox () {}

        // Adding epsilon threshold to avoid degenerated bounding boxes.
        inline void init (const glm::vec3 & p) { m_min = p - glm::vec3 (BBOX_EPSILON); m_max = p + glm::vec3 (BBOX_EPSILON); }
        
        inline const glm::vec3 & min () const { return m_min; }
        
        inline const glm::vec3 & max () const { return m_max; }
        
        inline glm::vec3 center () const { return (m_min + m_max) / 2.f; }

        inline float range (size_t axis) const { return m_max[axis] - m_min[axis]; }
        
        inline float width () const { return (m_max.x - m_min.x); }
        
        inline float height () const { return (m_max.y - m_min.y); }
        
        inline float length () const { return (m_max.z - m_min.z); }
        
        inline float size () const { return std::max (width (), std::max (height (), length ())); }
        
        inline float radius () const { return glm::distance (m_min, m_max) / 2.f; }
        
        inline float volume () const { return width () * height () * length (); }

        inline bool contains (const BoundingBox & b) const { return contains (b.m_min) && contains (b.m_max); }

        /// Returns 0 is the box is the largest along the X axis, 1 along the Y axis, and 2 along the Z axis.
        size_t dominantAxis () const;
        
        bool contains (const glm::vec3 & p) const ;
        
        void extendTo (const glm::vec3 & p) ;
        
        void extendTo (const BoundingBox & b);
        
        void translate (const glm::vec3 & t);
        
        void scale (float factor);
        
        bool intersect (const BoundingBox & b) const;
        
    private:
        static inline bool isIn (float x, float min, float max) { return (x >= min && x <= max); }

        glm::vec3 m_min;
        glm::vec3 m_max;
};