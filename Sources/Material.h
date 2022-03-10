// Fichier Material.h
#pragma once

#include <glad/glad.h>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>
#include <iostream>


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

    //GLuint loadTextureFromFileToGPU (const std::string & filename);

private:
    glm::vec3 m_albedo;
    float m_roughness;
    float m_metallicness;
};