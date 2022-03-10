#version 450 core // Minimal GL version support expected from the GPU

#define PI 3.1415

uniform mat4 normalMat, modelViewMat;

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


// Material
struct Material {
    vec3 albedo;
    float roughness;
    float metallicness;
};
uniform Material material;


// IN - OUT
layout(location=0) in vec3 fNormal; // Shader input, linearly interpolated by default from the previous stage (here the vertex shader)
layout(location=1) in vec3 fPosition;
layout(location=2) in vec2 fTexCoord;


layout(location = 0) out vec3 color;


vec3 get_fd() {
	return material.albedo / PI;
}

vec3 get_fs(vec3 w0, vec3 wi, vec3 wh, vec3 n) {
	float alpha = material.roughness;
	float alpha2 = pow(alpha, 2);
		
	float n_wh2 = pow(max(0., dot(n, wh)), 2);
	float n_wi = dot(n, wi);
	float n_w0 = dot(n, w0);
	float wi_wh = max(0., dot(wi, wh));
	float D = alpha2 /(PI * pow(1 + (alpha2 - 1) * n_wh2, 2));
		
	vec3 F0 = material.albedo + (vec3(1.) - material.albedo) * material.metallicness;
	vec3 F = F0 - (vec3(1.) - F0) * pow(1. - wi_wh, 5);
		
	float G1 = 2 * n_wi/(n_wi+sqrt(alpha2+(1-alpha2)*pow(n_wi,2)));
	float G2 = 2 * n_w0/(n_w0+sqrt(alpha2+(1-alpha2)*pow(n_w0,2)));
	float G = G1 * G2;
		
	vec3 fs = D*F*G/(4*n_wi*n_w0);
	return fs;
}

vec3 get_r(vec3 lightDirection, float lightIntensity, vec3 lightColor) {
	vec3 w0 = - normalize(fPosition);
	vec3 wi = normalize(lightDirection);
	vec3 wh = normalize(wi + w0);

	vec3 n = normalize(fNormal);
	vec3 fs = get_fs(w0, wi, wh, n);
	vec3 fd = get_fd();

	float scalarProd = max(0.0, dot(n, wi));
	vec3 luminosity = lightIntensity * lightColor;
		
	return luminosity * (fd + fs) * scalarProd;
}


void main () {
	vec3 r = vec3(0);
	for(int i=0; i<nb_lightsourcesDir; i++) {
		vec3 lightDirection = normalize(vec3(normalMat * vec4(lightsourcesDir[i].direction, 1.0)));
		r += get_r(-lightDirection, lightsourcesDir[i].intensity, lightsourcesDir[i].color);
	}
	
	for(int i=0; i<nb_lightsourcesPoint; i++) {
		vec3 lightDirection = vec3(modelViewMat * vec4(lightsourcesPoint[i].position, 1.0)) - fPosition;
		float d = length(lightDirection);
		float Li = lightsourcesPoint[i].intensity / (lightsourcesPoint[i].a_c + lightsourcesPoint[i].a_l * d + lightsourcesPoint[i].a_q *d*d);

		r += get_r(lightDirection, Li, lightsourcesPoint[i].color);
	}
	
	color = r;

	/*
	colorResponse = vec4(vec3(fTexCoord, 0.0f), 1.0f);
	
	int xx = int(fTexCoord[0] * 10.0f);
	int yy = int(fTexCoord[1] * 10.0f);
	if((xx + yy) % 2 == 0)
		colorResponse = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	else if((xx + yy) % 2 == 1)
		colorResponse = vec4(0.0f, 0.0f, 1.0f, 1.0f);*/
}