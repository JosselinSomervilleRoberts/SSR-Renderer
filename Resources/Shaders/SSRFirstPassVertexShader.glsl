#version 450 core // Minimal GL version support expected from the GPU

layout(location=0) in vec3 vPosition; // The 1st input attribute is the position (CPU side: glVertexAttrib 0)
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoord;

uniform mat4 projectionMat, modelViewMat, normalMat; // Uniform variables, set from the CPU-side main program

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

void main() {
	vec4 p = modelViewMat * vec4 (vPosition, 1.0);
    vec4 n = normalMat * vec4 (normalize (vNormal), 1.0);
    
    Normal = normalize (n.xyz);
    FragPos = p.xyz;
    TexCoords = vTexCoord;
    
    gl_Position =  projectionMat * p; // mandatory to fire rasterization properly
}