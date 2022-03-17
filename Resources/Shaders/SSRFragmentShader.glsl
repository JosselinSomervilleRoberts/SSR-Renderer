#version 450 core

uniform int diagnostic = 1;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gRendered;
uniform mat4 projectionMat, normalMat, modelViewMat;
in vec2 TexCoords;

out vec4 FragColor;




struct Ray
{
	vec3 o;
	vec3 viewNormal;
	vec3 d;
	vec3 currPos;
	float steps;
	vec2 uv;
	float diff_depth;

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
Ray TraceRay2(vec2 uv, int nbSteps, float start, float end);
Ray TraceRay(vec2 uv);
Ray BinarySearch(Ray ray, float stepSize);

// Shading
vec3 get_fd(vec3 albedo);
vec3 get_fs(vec3 w0, vec3 wi, vec3 wh, vec3 n, vec3 albedo, float roughness, float metalicness);
vec3 get_r(vec3 position, vec3 normal, vec3 lightDirection, float lightIntensity, vec3 lightColor, vec3 albedo, float roughness, float metalicness);

float minVec3(vec3 v) { return min(v.x, min(v.y, v.z)); }
float maxVec3(vec3 v) { return max(v.x, max(v.y, v.z)); }


int maxSteps = 40;
int binarySearchSteps = 5;
float SSRinitialStep = 0.01f;
float SSRstep = 0.1f;


Ray TraceRay(vec2 uv) {
	return TraceRay2(uv, maxSteps, SSRinitialStep, SSRinitialStep + SSRstep * (maxSteps - 1));
}

Ray TraceRay2(vec2 uv, int nbSteps, float start, float end) {
	// First compute the step
	float stepSize = (end - start) / (nbSteps - 1);

    // Everything is computed in the view space
	Ray ray;
    ray.o = getPosition(uv);
    vec3 V = normalize(ray.o); // Frag pos - camera (and camera = (0,0,0) in camera space
    ray.viewNormal = normalize(getNormal(uv));
    ray.d = normalize(reflect(V, ray.viewNormal));

	// iniate ray march
    ray.out_col = vec3(0.0);
	ray.hit = false;
	ray.steps = 0;
	ray.currPos = ray.o + ray.d * start;

	// Instantiate variables
	float rayDepth;
	vec2 pixelUV;
	float pixelDepth;
	float prev_diff_depth = 0;
	bool resized = false;
	float stepIncrease = 1;
	int countResized = 0;
	int countResizedMax = 10;


	while(ray.steps < nbSteps)
	{
		if(resized) {
			countResized++;
			if(countResized > countResizedMax) {
				resized = false;
				stepIncrease *= countResizedMax;
			}
		}

		// Advance in the ray
		ray.currPos += ray.d * stepSize * stepIncrease;
		ray.steps += stepIncrease;
		rayDepth = length(ray.currPos);

		// Get the depth
		pixelUV = getUV(ray.currPos);
		pixelDepth = getDepth(pixelUV);

		ray.diff_depth = pixelDepth - rayDepth;

		if(abs(ray.diff_depth) < 2.0f * stepSize * stepIncrease) {
			if(diagnostic != 7) ray = BinarySearch(ray, stepSize);
			if(abs(ray.diff_depth) < 0.2f * stepSize * stepIncrease) {
				ray.uv = getUV(ray.currPos);
				ray.out_col = getAlbedo(ray.uv);
				ray.hit = true;
				return ray;
			}
		} /*
		else if((resized == false) &&(diagnostic == 7) && (diff_depth * prev_diff_depth < -0.01f)) {
			resized = true;
			ray.currPos -= ray.d * stepSize * stepIncrease;
			ray.steps -= stepIncrease;
			stepIncrease /= float(countResizedMax);
			countResized = 0;
		}*/
		else
			prev_diff_depth = ray.diff_depth;
	}

    return ray;
}

Ray BinarySearch(Ray ray, float stepSize) {
	float rayDepth;
	vec2 pixelUV;
	float pixelDepth;

	for (int i=0; i<binarySearchSteps; i++) {
		stepSize *= 0.5f;
		rayDepth = length(ray.currPos);
		pixelUV = getUV(ray.currPos);
		pixelDepth = getDepth(pixelUV);
		ray.diff_depth = pixelDepth - rayDepth;

		if (ray.diff_depth > 0.0f)
			ray.currPos += ray.d * stepSize;
		else
			ray.currPos -= ray.d * stepSize;
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

vec3 getRendered(vec2 coords) {
	return texture2D(gRendered, coords).rgb; 
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
