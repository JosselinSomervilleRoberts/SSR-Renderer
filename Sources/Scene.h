// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "Camera.h"
#include "Mesh.h"
#include "Material.h"

#include "Light/LightSourceDir.h"
#include "Light/LightSourcePoint.h"


class Scene {
public:
	inline Scene () : m_backgroundColor (0.f, 0.f ,0.f) {
		//auto materialPtr = std::make_shared<Material> ();
		//this->addMaterial(materialPtr);
	}
	virtual ~Scene() {}

	// Background
	inline const glm::vec3 & backgroundColor () const { return m_backgroundColor; }
	inline void setBackgroundColor (const glm::vec3 & color) { m_backgroundColor = color; }
 
	// Camera
	inline void set (std::shared_ptr<Camera> camera) { m_camera = camera; }
	inline const std::shared_ptr<Camera> camera() const { return m_camera; }
	inline std::shared_ptr<Camera> camera() { return m_camera; }

	// Mesh
	inline void add (std::shared_ptr<Mesh> mesh) { m_meshes.push_back (mesh); }
	inline size_t numOfMeshes () const { return m_meshes.size (); }
	inline const std::shared_ptr<Mesh> mesh (size_t index) const { return m_meshes[index]; }
	inline std::shared_ptr<Mesh> mesh (size_t index) { return m_meshes[index]; }

	// Material
	inline void addMaterial (std::shared_ptr<Material> material) { m_materials.push_back (material); }
	inline size_t numOfMaterials () const { return m_materials.size (); }
	inline const std::shared_ptr<Material> material (size_t index) const { return m_materials[index]; }
	inline std::shared_ptr<Material> material (size_t index) { return m_materials[index]; }
	inline void setMaterialToMesh(size_t indexMesh, size_t indexMaterial) { this->m_mesh2material[indexMesh] = indexMaterial; }
	inline size_t getMaterialOfMesh(size_t indexMesh) { if (m_mesh2material.find(indexMesh) == m_mesh2material.end()) { return 0; } else { return m_mesh2material[indexMesh]; }}

	// Lightsource
	inline void addLightSource (std::shared_ptr<LightSourceDir>   lightSource) { m_lightSourcesDir.push_back (lightSource); }
	inline void addLightSource (std::shared_ptr<LightSourcePoint> lightSource) { m_lightSourcesPoint.push_back (lightSource); }
	inline size_t numOfLightSourcesDir   () const { return m_lightSourcesDir.size (); }
	inline size_t numOflightSourcesPoint () const { return m_lightSourcesPoint.size (); }
	inline const std::shared_ptr<LightSourceDir> lightSourceDir (size_t index) const { return m_lightSourcesDir[index]; }
	inline std::shared_ptr<LightSourceDir> lightSourceDir (size_t index) { return m_lightSourcesDir[index]; }
	inline const std::shared_ptr<LightSourcePoint> lightSourcePoint (size_t index) const { return m_lightSourcesPoint[index]; }
	inline std::shared_ptr<LightSourcePoint> lightSourcePoint (size_t index) { return m_lightSourcesPoint[index]; }

	inline void clear () {
		m_camera.reset ();
		m_meshes.clear ();
	}

private:
	glm::vec3 m_backgroundColor;
	std::shared_ptr<Camera> m_camera;

	// Objects
	std::vector<std::shared_ptr<Mesh> > m_meshes;
	std::vector<std::shared_ptr<Material> > m_materials;
	std::unordered_map<size_t, size_t> m_mesh2material;

	// Lights
	std::vector<std::shared_ptr<LightSourceDir> > m_lightSourcesDir;
	std::vector<std::shared_ptr<LightSourcePoint> > m_lightSourcesPoint;
};