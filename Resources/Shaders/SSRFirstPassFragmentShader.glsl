#version 330 core
#define PI 3.14159


layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gRendered;

uniform mat4 modelViewMat, normalMat;

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

// Lights
struct LightSourceDir {
	vec3 direction;
	float intensity;
	vec3 color;
};
uniform LightSourceDir lightsourcesDir[8];
uniform int nb_lightsourcesDir;


struct LightSourcePoint {
	vec3 position;
	float intensity;
	vec3 color;
	float a_c;
    float a_l;
    float a_q;
};
uniform LightSourcePoint lightsourcesPoint[8];
uniform int nb_lightsourcesPoint;



vec3 get_fd(vec3 albedo) {
	return albedo / PI;
}

vec3 get_fs(vec3 w0, vec3 wi, vec3 wh, vec3 n, vec3 albedo, float roughness, float metalicness) {
	float alpha = roughness;
	float alpha2 = pow(alpha, 2);
		
	float n_wh2 = pow(max(0., dot(n, wh)), 2);
	float n_wi = dot(n, wi);
	float n_w0 = dot(n, w0);
	float wi_wh = max(0., dot(wi, wh));
	float D = alpha2 /(PI * pow(1 + (alpha2 - 1) * n_wh2, 2));
		
	vec3 F0 = albedo + (vec3(1.) - albedo) * metalicness;
	vec3 F = F0 - (vec3(1.) - F0) * pow(1. - wi_wh, 5);
		
	float G1 = 2 * n_wi/(n_wi+sqrt(alpha2+(1-alpha2)*pow(n_wi,2)));
	float G2 = 2 * n_w0/(n_w0+sqrt(alpha2+(1-alpha2)*pow(n_w0,2)));
	float G = G1 * G2;
		
	vec3 fs = D*F*G/(4*n_wi*n_w0);
	return fs;
}

vec3 get_r(vec3 position, vec3 normal, vec3 lightDirection, float lightIntensity, vec3 lightColor, vec3 albedo, float roughness, float metalicness) {
	vec3 w0 = - normalize(position);
	vec3 wi = normalize(lightDirection);
	vec3 wh = normalize(wi + w0);
	vec3 n = normalize(normal);

	vec3 fs = get_fs(w0, wi, wh, n, albedo, roughness, metalicness);
	vec3 fd = get_fd(albedo);

	float scalarProd = max(0.0, dot(n, wi));
	vec3 luminosity = lightIntensity * lightColor;
		
	return luminosity * (fd + fs) * scalarProd;
}

vec3 getRendered(vec3  fragPos, vec3  fragNormal, vec3  fragAlbedo, float fragMetalicness, float fragRoughness) {
	vec3 r = vec3(0);
	for(int i=0; i<nb_lightsourcesDir; i++) {
		vec3 lightDirection = normalize(vec3(normalMat * vec4(lightsourcesDir[i].direction, 1.0)));
		r += get_r(fragPos, fragNormal, -lightDirection, lightsourcesDir[i].intensity, lightsourcesDir[i].color, fragAlbedo, fragRoughness, fragMetalicness);
	}
	
	for(int i=0; i<nb_lightsourcesPoint; i++) {
		vec3 lightDirection = vec3(modelViewMat * vec4(lightsourcesPoint[i].position, 1.0)) - fragPos;
		float d = length(lightDirection);
		float Li = lightsourcesPoint[i].intensity / (lightsourcesPoint[i].a_c + lightsourcesPoint[i].a_l * d + lightsourcesPoint[i].a_q *d*d);

		r += get_r(fragPos, fragNormal, lightDirection, Li, lightsourcesPoint[i].color, fragAlbedo, fragRoughness, fragMetalicness);
	}

	return r;
}

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition.xyz = FragPos;
    gPosition.w   = Depth;
    // also store the per-fragment normals into the gbuffer
	vec3 n = normalize(Normal);
    gNormal.xyz = n;
	gNormal.w = material.roughness;

    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = material.albedo;
    gAlbedoSpec.a = material.metallicness;

	gRendered = vec4(getRendered(FragPos, n, material.albedo, material.metallicness, material.roughness), 1.0f);
}  