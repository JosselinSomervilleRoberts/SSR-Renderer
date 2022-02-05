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

#define PI 3.1415


RayTracer::RayTracer() : 
	m_imagePtr (std::make_shared<Image>()) {}

RayTracer::~RayTracer() {}

void RayTracer::init (const std::shared_ptr<Scene> scenePtr) {
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

	for(size_t x=0; x<width; x++) {
		for(size_t y=0; y<height; y++) {
			float posX = x / (float)(width  - 1);
			float posY = y / (float)(height - 1);

			Ray ray = scenePtr->camera()->rayAt(posX, posY);
			float e = std::numeric_limits<float>::max();

			//std::cout << ray.origin.x << ", " << ray.origin.y << ", " << ray.origin.z  << " / " << ray.direction.x << ", " << ray.direction.y << ", " << ray.direction.z << std::endl;
			
			for (size_t i = 0; i < numOfMeshes; i++) {
				const std::shared_ptr<Mesh> mesh = scenePtr->mesh(i);

				const std::vector<glm::vec3> vertexPositions  = mesh->vertexPositions();
				const std::vector<glm::uvec3> triangleIndices = mesh->triangleIndices();
				const size_t nbTriangles = triangleIndices.size();

				for(size_t k=0; k<nbTriangles; k++) {
					const glm::uvec3 trianglePos = triangleIndices[k];
					const glm::vec3 p0 = glm::vec3(scenePtr->camera()->computeViewMatrix() * glm::vec4(vertexPositions[trianglePos[0]], 1.0f));
					const glm::vec3 p1 = glm::vec3(scenePtr->camera()->computeViewMatrix() * glm::vec4(vertexPositions[trianglePos[1]], 1.0f));
					const glm::vec3 p2 = glm::vec3(scenePtr->camera()->computeViewMatrix() * glm::vec4(vertexPositions[trianglePos[2]], 1.0f));
					Triangle triangle(p0, p1, p2);

					RayHit* rayHit = rayIntersect(ray, triangle);

					if(rayHit != nullptr) {
						std::cout << "intersection";
						float d = glm::distance(camPos, rayHit->hitPosition());
						if(d < e) {
							e = d;
							m_imagePtr->operator()(x,y) = shade(rayHit, scenePtr);// TODO shade(rayHit)
							delete rayHit;
						}
					}
				}
			}
		}
	}

	std::chrono::time_point<std::chrono::high_resolution_clock> after = clock.now();
	double elapsedTime = (double)std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count();
	Console::print ("Ray tracing executed in " + std::to_string(elapsedTime) + "ms");
}


RayHit* RayTracer::rayIntersect(Ray& ray, Triangle& triangle) {
	float epsilon = 0.00001f;

	glm::vec3 e0 = triangle.p1 - triangle.p0;
	glm::vec3 e1 = triangle.p2 - triangle.p0;
	glm::vec3 n = glm::normalize(glm::cross(e0, e1));
	glm::vec3 q = glm::cross(ray.direction, e1);
	float a = glm::dot(e0, q);

	if((glm::dot(n, ray.direction) >= 0) || (std::abs(a) < epsilon))
		return nullptr;

	glm::vec3 s = (ray.origin - triangle.p0) / a;
	glm::vec3 r = glm::cross(s, e0);

	float b0 = glm::dot(s, q);
	float b1 = glm::dot(r, ray.direction);
	float b2 = 1 - b0 - b1;

	if ((b0 < 0) || (b1 < 0) || (b2 < 0))
		return nullptr;

	float t = glm::dot(e1, r);

	if (t >= 0)
		return new RayHit(triangle, b0, b1, b2, t);

	return nullptr;
}


glm::vec3 RayTracer::shade(RayHit* rayHit, const std::shared_ptr<Scene> scenePtr) {
	const size_t numOfLightSourcesDir = scenePtr->numOfLightSourcesDir();

	for(size_t i=0; i<numOfLightSourcesDir; i++) {

	}

	return glm::vec3(1., 0., 0.);
}

glm::vec3 RayTracer::get_fd(Material& material) {
	return material.albedo() / (float)(PI);
}

glm::vec3 RayTracer::get_fs(Material& material, glm::vec3 w0, glm::vec3 wi, glm::vec3 wh, glm::vec3 n) {
	float alpha = material.roughness();
	float alpha2 = pow(alpha, 2);
		
	float n_wh2 = pow(std::max(0.0f, dot(n, wh)), 2);
	float n_wi = glm::dot(n, wi);
	float n_w0 = glm::dot(n, w0);
	float wi_wh = std::max(0.0f, glm::dot(wi, wh));
	float D = alpha2 / (PI * pow(1 + (alpha2 - 1) * n_wh2, 2));
		
	glm::vec3 F0 = material.albedo() + (glm::vec3(1.) - material.albedo()) * material.metallicness();
	glm::vec3 F = F0 - (float)(pow(1.0f - wi_wh, 5)) * (glm::vec3(1.0f) - F0);
		
	float G1 = 2.0f * n_wi / (n_wi+sqrt(alpha2+(1-alpha2)*pow(n_wi,2)));
	float G2 = 2.0f * n_w0 / (n_w0+sqrt(alpha2+(1-alpha2)*pow(n_w0,2)));
	float G = G1 * G2;
		
	glm::vec3 fs = D*F*G/(4*n_wi*n_w0);
	return fs;
}

glm::vec3 RayTracer::get_r(Material& material, glm::vec3 fPosition, glm::vec3 fNormal, glm::vec3 lightDirection, float lightIntensity, glm::vec3 lightColor) {
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