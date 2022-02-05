// Fichier Material.h
#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>


class Material {
public:
    Material (const glm::vec3& diffuseAlbedo = glm::vec3(0.5f, 0.5f, 0.5f), float roughness = 0.15f, float metallicness = 0.9f)
    : m_albedo (diffuseAlbedo),
      m_roughness (roughness),
      m_metallicness (metallicness) {}
    virtual ~Material () {}

    inline glm::vec3 albedo () const { return m_albedo; }
    inline void setAlbedo (glm::vec3 albedo) { m_albedo = albedo; }
    inline float roughness () const { return m_roughness; }
    inline void setRoughness (float roughness) { m_roughness = roughness; }
    inline float metallicness () const { return m_metallicness; }
    inline void setMetallicness (float metallicness) { m_metallicness = metallicness; }

private:
    glm::vec3 m_albedo;
    float m_roughness;
    float m_metallicness;
};