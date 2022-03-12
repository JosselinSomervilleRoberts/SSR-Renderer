// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#pragma once

#include <glad/glad.h>
#include <string>

#include "Scene.h"
#include "Mesh.h"
#include "Image.h"
#include "ShaderProgram.h"
#include "Rasterizer.h"

class RasterizerSSR : public Rasterizer {
public:

	inline RasterizerSSR () : Rasterizer() {}

	virtual ~RasterizerSSR () {}

	/// OpenGL context, shader pipeline initialization and GPU ressources (vertex buffers, textures, etc)
	void init (const std::string & basepath, const std::shared_ptr<Scene> scenePtr);
	void setResolution (int width, int height);

	/// Loads and compile the programmable shader pipeline
	void loadShaderProgram (const std::string & basePath);
	void render (std::shared_ptr<Scene> scenePtr);
    void renderQuad();

private:
	GLuint genGPUBuffer (size_t elementSize, size_t numElements, const void * data);
	void initScreenQuad ();

	std::shared_ptr<ShaderProgram> shaderFirstPass; // A GPU program contains at least a vertex shader and a fragment shader
	std::shared_ptr<ShaderProgram> shaderSecondPass; // Full screen quad shader program, for displaying 2D color images

    unsigned int SCR_WIDTH = 1024;
    unsigned int SCR_HEIGHT = 768;

    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    unsigned int gPosition, gNormal, gAlbedoSpec;
    unsigned int gBuffer;
};