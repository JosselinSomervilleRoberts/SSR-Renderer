#version 450 core
#define PI 3.14159

uniform int diagnostic = 1;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform mat4 projectionMat, normalMat, modelViewMat;
in vec2 TexCoords;

out vec4 FragColor;

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


struct Ray
{
	vec3 o;
	vec3 viewNormal;
	vec3 d;
	vec3 currPos;
	int steps;
	vec2 uv;

	vec3 out_col;
	bool hit;
};


// SSR
float getDepth(vec2 coords);
vec3  getPosition(vec2 coords);
vec3  getNormal(vec2 coords);
vec3  getAlbedo(vec2 coords);
float getRoughness(vec2 coords);
float getMetalicness(vec2 coords);
vec2  getUV(vec3 pos);
Ray TraceRay(vec2 uv);

// Shading
vec3 get_fd(vec3 albedo);
vec3 get_fs(vec3 w0, vec3 wi, vec3 wh, vec3 n, vec3 albedo, float roughness, float metalicness);
vec3 get_r(vec3 position, vec3 normal, vec3 lightDirection, float lightIntensity, vec3 lightColor, vec3 albedo, float roughness, float metalicness);

float minVec3(vec3 v) { return min(v.x, min(v.y, v.z)); }
float maxVec3(vec3 v) { return max(v.x, max(v.y, v.z)); }


int maxSteps = 700;
float SSRinitialStepAmount = 0.003f;

/*
vec3 calcViewPosition(in vec2 texCoord) {
    // Combine UV & depth into XY & Z (NDC)
    vec3 rawPosition = vec3(texCoord, getDepth(texCoord));

    // Convert from (0, 1) range to (-1, 1)
    vec4 ScreenSpacePosition = vec4(rawPosition * 2 - 1, 1);

    // Undo Perspective transformation to bring into view space
    vec4 ViewPosition = inv_projection * ScreenSpacePosition;

    // Perform perspective divide and return
    return ViewPosition.xyz / ViewPosition.w;
}*/

Ray TraceRay(vec2 uv) {
	Ray ray;

    // Everything is computed in the view space
    ray.o = getPosition(uv);
    vec3 V = normalize(ray.o); // Frag pos - camera (and camera = (0,0,0) in camera space
    ray.viewNormal = normalize(getNormal(uv));
    ray.d = normalize(reflect(V, ray.viewNormal));

	// Ray march in screen space.
    ray.out_col = vec3(0.0);
	ray.hit = false;
	ray.steps = 0;

	while(ray.steps < maxSteps)
	{
		// Advance in the ray
		ray.steps++;
		ray.currPos = ray.o + ray.steps * ray.d * SSRinitialStepAmount;
		float rayDepth = length(ray.currPos);

		// Get the depth
		vec2 pixelUV = getUV(ray.currPos);
		float pixelDepth = getDepth(pixelUV);

		if(abs(pixelDepth - rayDepth) < 0.01f) {
			ray.out_col = getAlbedo(pixelUV);
			//if (length(ray.out_col) > 0.05f) {
				ray.hit = true;
				ray.uv = pixelUV;
				return ray;
			//}
		}
	}

    return ray;
}

float getDepth(vec2 coords) {
    return texture2D(gPosition, coords).w; 
}

vec3 getPosition(vec2 coords) {
    return texture2D(gPosition, coords).xyz;
}

vec3 getNormal(vec2 coords) {
    return texture2D(gNormal, coords).xyz;
}

vec3 getAlbedo(vec2 coords) {
    return texture2D(gAlbedoSpec, coords).xyz; 
}

float getRoughness(vec2 coords) {
    return texture2D(gNormal, coords).w; 
}

float getMetalicness(vec2 coords) {
    return texture2D(gAlbedoSpec, coords).w; 
}

vec2 getUV(vec3 pos) {
    vec4 projectedPos = projectionMat * vec4(pos, 1.0f);
    projectedPos.xy /= projectedPos.w;
    projectedPos.xy = projectedPos.xy * 0.5f +0.5f;
    return projectedPos.xy;
}



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

vec3 getRendered(vec2 coords) {
	vec3  fragPos         = getPosition(coords);
    vec3  fragNormal      = getNormal(coords);
    vec3  fragAlbedo      = getAlbedo(coords);
    float fragMetalicness = getMetalicness(coords);
    float fragRoughness   = getRoughness(coords);

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

float reflectionSpecularFalloffExponent = 3.0;
void main () {
	vec3 RENDER = getRendered(TexCoords);
	float SSR_multiplier = 0.0f;
	vec3 SSR = vec3(0);

    Ray ray = TraceRay(TexCoords);

    float fragMetalicness = getMetalicness(TexCoords);
    vec3 fragNormal = getNormal(TexCoords);
    vec3 fragAlbedo = getAlbedo(TexCoords);

	if(ray.hit == true) {
		vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - ray.uv));
		float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0f, 1.0f);
		float multiplier = pow(fragMetalicness, reflectionSpecularFalloffExponent) * screenEdgefactor * length(ray.d);
		SSR_multiplier = clamp(multiplier, 0.0f, 0.9f);
		SSR = getRendered(ray.uv);
	}

	vec3 r = RENDER + (SSR - RENDER) * SSR_multiplier;
	//if (ray.hit) r = vec3(1,0,0);
	
    
    if (diagnostic == 1)
		FragColor = vec4 (r, 1.0);
	else if(diagnostic == 2)
		FragColor = vec4 (fragNormal, 1.0);
	else if(diagnostic == 3)
		FragColor = vec4(fragAlbedo, 1);
	else if(diagnostic == 4) {
		int xx = int(TexCoords[0] * 10.0f);
		int yy = int(TexCoords[1] * 10.0f);
		if((xx + yy) % 2 == 0)
			FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		else if((xx + yy) % 2 == 1)
			FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	else if(diagnostic == 5)
		FragColor = vec4(ray.d, 1);
	else if(diagnostic == 6) {
		if (ray.hit) FragColor = vec4(ray.out_col, 1);
        else FragColor = vec4 (r, 1.0);;
    }
	else {
		FragColor = vec4 (r, 1.0);
	}
	/*
    if (ray.hit) {
        FragColor  = ray.out_col;  
    }*/

	/*
	colorResponse = vec4(vec3(fTexCoord, 0.0f), 1.0f);
	
	int xx = int(fTexCoord[0] * 10.0f);
	int yy = int(fTexCoord[1] * 10.0f);
	if((xx + yy) % 2 == 0)
		colorResponse = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	else if((xx + yy) % 2 == 1)
		colorResponse = vec4(0.0f, 0.0f, 1.0f, 1.0f);*/
}
