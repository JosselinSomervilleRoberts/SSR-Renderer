#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in float Depth;

// Material
struct Material {
    vec3 albedo;
    float roughness;
    float metallicness;
};
uniform Material material;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition.xyz = FragPos;
    gPosition.w   = Depth;
    // also store the per-fragment normals into the gbuffer
    gNormal.xyz = normalize(Normal);
	gNormal.w = material.roughness;

    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = material.albedo;
    gAlbedoSpec.a = material.metallicness;
}  