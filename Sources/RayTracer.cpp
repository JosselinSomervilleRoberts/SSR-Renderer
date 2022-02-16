// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#include "RayTracer.h"

#include "Console.h"
#include "Camera.h"

#define PI 3.1415f


RayTracer::RayTracer() : 
	m_imagePtr (std::make_shared<Image>()) {}

RayTracer::~RayTracer() {
}

void RayTracer::init (const std::shared_ptr<Scene> scenePtr) {
	bvh.init(scenePtr);
}

void RayTracer::render (const std::shared_ptr<Scene> scenePtr) {
	size_t width = m_imagePtr->width();
	size_t height = m_imagePtr->height();
	std::chrono::high_resolution_clock clock;
	Console::print ("Start ray tracing at " + std::to_string (width) + "x" + std::to_string (height) + " resolution...");
	std::chrono::time_point<std::chrono::high_resolution_clock> before = clock.now();
	m_imagePtr->clear (scenePtr->backgroundColor ());
	//m_imagePtr->operator()(10, 10) = glm::vec3(1.0, 0.0, 0.0);
	
	// <---- Ray tracing code ---->
	size_t numOfMeshes = scenePtr->numOfMeshes ();
	glm::vec3 camPos = scenePtr->camera()->getPosition();
	
	RayHit rayHit = RayHit(0, 0, 0, 0);
	Ray ray;

	// Precomputation
	glm::vec3 viewRight,  viewUp,  viewDir,  eye;
    float w;
	std::vector<glm::mat4> modelViewMats;
	std::vector<glm::mat4> normalMats;
    if(useBVH) {
		scenePtr->camera()->computeVectorsForRayAt(viewRight, viewUp, viewDir, eye, w);
	
		for (size_t i = 0; i < numOfMeshes; i++) {
			const std::shared_ptr<Mesh>& mesh = scenePtr->mesh(i);
			glm::mat4 modelMat = mesh->computeTransformMatrix ();
			glm::mat4 viewMat = scenePtr->camera()->computeViewMatrix ();
			glm::mat4 modelViewMat = viewMat * modelMat;
			glm::mat4 normalMat = glm::transpose (glm::inverse (modelViewMat));

			modelViewMats.push_back(modelViewMat);
			normalMats.push_back(normalMat);
		}
	}

	for(size_t x=0; x<width; x++) {
		for(size_t y=0; y<height; y++) {
			float posX = x / (float)(width  - 1);
			float posY = 1 - (y / (float)(height - 1));

			rayHit.t = std::numeric_limits<float>::max();

			if (useBVH) {
				ray = scenePtr->camera()->rayAt(posX, posY, viewRight, viewUp, viewDir, eye, w);
				size_t mesh_index = 0;
				size_t triangle_index = 0;
				bool hit = bvh.intersect(scenePtr, rayHit, ray, mesh_index, triangle_index);
				if(hit) m_imagePtr->operator()(x,y) = shade(scenePtr, rayHit, mesh_index, triangle_index, modelViewMats[mesh_index], normalMats[mesh_index]);
			}
			else {
				ray = scenePtr->camera()->rayAt(posX, posY);
				for (size_t i = 0; i < numOfMeshes; i++) {
					const std::shared_ptr<Mesh>& mesh = scenePtr->mesh(i);

					const std::vector<glm::vec3>& vertexPositions  = mesh->vertexPositions();
					const std::vector<glm::uvec3>& triangleIndices = mesh->triangleIndices();
					const size_t nbTriangles = triangleIndices.size();

					for(size_t k=0; k<nbTriangles; k++) {
						const glm::uvec3& trianglePos = triangleIndices[k];
						const glm::vec3& p0 = vertexPositions[trianglePos[0]];
						const glm::vec3& p1 = vertexPositions[trianglePos[1]];
						const glm::vec3& p2 = vertexPositions[trianglePos[2]];
						
						bool hit = ray.intersect(rayHit, p0, p1, p2);
						if(hit) m_imagePtr->operator()(x,y) = shade(scenePtr, rayHit, i, k);
					}
				}
			}

			
		}
	}

	std::chrono::time_point<std::chrono::high_resolution_clock> after = clock.now();
	double elapsedTime = (double)std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count();
	Console::print ("Ray tracing executed in " + std::to_string(elapsedTime) + "ms");
}



glm::vec3 RayTracer::shade(const std::shared_ptr<Scene> scenePtr, RayHit& rayHit, size_t& mesh_index, size_t& triangle_index) {
	const std::shared_ptr<Mesh>& mesh = scenePtr->mesh(mesh_index);
	glm::mat4 modelMat = mesh->computeTransformMatrix ();
	glm::mat4 viewMat = scenePtr->camera()->computeViewMatrix ();
	glm::mat4 modelViewMat = viewMat * modelMat;
	glm::mat4 normalMat = glm::transpose (glm::inverse (modelViewMat));

	return shade(scenePtr, rayHit, mesh_index, triangle_index, modelViewMat, normalMat);
}

glm::vec3 RayTracer::shade(const std::shared_ptr<Scene> scenePtr, RayHit& rayHit, size_t& mesh_index, size_t& triangle_index, glm::mat4& modelViewMat, glm::mat4& normalMat) {
	// To compute the shading
	const std::shared_ptr<Mesh>& mesh = scenePtr->mesh(mesh_index);
	size_t materialIndex = scenePtr->getMaterialOfMesh(mesh_index);
	std::shared_ptr<Material> material  = scenePtr->material(materialIndex);
	const std::vector<glm::vec3>& vertexPositions  = mesh->vertexPositions();
	const std::vector<glm::vec3>& vertexNormals    = mesh->vertexNormals();
	const std::vector<glm::uvec3>& triangleIndices = mesh->triangleIndices();
	const glm::uvec3& trianglePos = triangleIndices[triangle_index];

	// fPosition
	const glm::vec3& p0 = vertexPositions[trianglePos[0]];
	const glm::vec3& p1 = vertexPositions[trianglePos[1]];
	const glm::vec3& p2 = vertexPositions[trianglePos[2]];
	glm::vec3 fPosition =  glm::vec3(modelViewMat * glm::vec4(rayHit.hitPosition(p1, p2, p0), 1.0f));

	// Normal
	const glm::vec3& n0 = vertexNormals[trianglePos[0]];
	const glm::vec3& n1 = vertexNormals[trianglePos[1]];
	const glm::vec3& n2 = vertexNormals[trianglePos[2]];
	const glm::vec3 vNormal = glm::normalize(rayHit.hitPosition(n1, n2, n0));
	glm::vec3 fNormal = glm::normalize(glm::vec3(normalMat * glm::vec4 (normalize (vNormal), 1.0)));


	const size_t numOfLightSourcesDir = scenePtr->numOfLightSourcesDir();
	glm::vec3 r = glm::vec3(0., 0., 0.);
	for(size_t i=0; i<numOfLightSourcesDir; i++) {
		auto lightSourcePtr = scenePtr->lightSourceDir(i);
		glm::vec3 lightDirection = glm::normalize(glm::vec3(normalMat * glm::vec4(lightSourcePtr->direction, 1.0)));
		r += get_r(material, fPosition, fNormal, -lightDirection, lightSourcePtr->intensity, lightSourcePtr->color);
	}

	return r;
}

glm::vec3 RayTracer::get_fd(std::shared_ptr<Material> material) {
	return material->albedo() / (float)(PI);
}

glm::vec3 RayTracer::get_fs(std::shared_ptr<Material> material, glm::vec3& w0, glm::vec3& wi, glm::vec3& wh, glm::vec3& n) {
	float alpha = material->roughness();
	float alpha2 = pow(alpha, 2.0f);
		
	float n_wh2 = pow(std::max(0.0f, dot(n, wh)), 2.0f);
	float n_wi = glm::dot(n, wi);
	float n_w0 = glm::dot(n, w0);
	float wi_wh = std::max(0.0f, glm::dot(wi, wh));
	float D = alpha2 / (PI * pow(1.0f + (alpha2 - 1.0f) * n_wh2, 2.0f));
		
	glm::vec3 F0 = material->albedo() + (glm::vec3(1.) - material->albedo()) * material->metallicness();
	glm::vec3 F = F0 - (float)(pow(1.0f - wi_wh, 5.0f)) * (glm::vec3(1.0f) - F0);
		
	float G1 = 2.0f * n_wi / (n_wi+sqrt(alpha2+(1-alpha2)*pow(n_wi, 2.0f)));
	float G2 = 2.0f * n_w0 / (n_w0+sqrt(alpha2+(1-alpha2)*pow(n_w0, 2.0f)));
	float G = G1 * G2;
		
	glm::vec3 fs = D*F*G/(4*n_wi*n_w0);
	return fs;
}

glm::vec3 RayTracer::get_r(std::shared_ptr<Material> material, glm::vec3& fPosition, glm::vec3& fNormal, glm::vec3& lightDirection, float& lightIntensity, glm::vec3& lightColor) {
	glm::vec3 w0 = - glm::normalize(fPosition);
	glm::vec3 wi = glm::normalize(lightDirection);
	glm::vec3 wh = glm::normalize(wi + w0);

	glm::vec3 n = glm::normalize(fNormal);
	glm::vec3 fs = get_fs(material, w0, wi, wh, n);
	glm::vec3 fd = get_fd(material);

	float scalarProd = max(0.0f, dot(n, wi));
	glm::vec3 luminosity = lightIntensity * lightColor;
		
	return luminosity * (fd + fs) * scalarProd;
}