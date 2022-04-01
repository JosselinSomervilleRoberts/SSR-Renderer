#version 450 core
#define PI 3.14159

uniform int diagnostic = 1;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform mat4 projectionMat, normalMat, modelViewMat;

uniform float extent;
uniform bool useBinary;
uniform bool useAntiAlias;
uniform bool useReflectedShading;
uniform bool useInTexture;
uniform bool allowBehindCamera;
uniform bool useScreenEdge;
uniform bool useDirectionShading;

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

// Ray structure
struct Ray
{
	// Definition of ray
	vec3 origin;
	vec3 viewNormal;
	vec3 direction;
	vec3 reflected;

	// To march in 3D
	vec3 currPos;
	vec3 increment;

	// To march in texture
	vec2 currPosUV;
	vec2 incrementUV;
	
	// Rasy status
	int steps;
	vec3 out_col;
	bool hit;
};


// Helpers for SSR
float getDepth(vec2 coords);
vec3  getPosition(vec2 coords);
vec3  getNormal(vec2 coords);
vec3  getAlbedo(vec2 coords);
float getRoughness(vec2 coords);
float getMetalicness(vec2 coords);
vec2  getUV(vec3 pos);

// Shading
vec3 get_fd(vec3 albedo);
vec3 get_fs(vec3 w0, vec3 wi, vec3 wh, vec3 n, vec3 albedo, float roughness, float metalicness);
vec3 get_r(vec3 position, vec3 normal, vec3 lightDirection, float lightIntensity, vec3 lightColor, vec3 albedo, float roughness, float metalicness);
vec3 getShading(vec3 fragPos, vec3 fragNormal, vec3 fragAlbedo, float fragRoughness, float fragMetalicness);
vec3 getRendered(vec2 coords);
vec3 SSR(vec2 texCoord, vec3 fragRendered);

// Ray Marching functions
Ray BinarySearch(Ray ray, vec3 startView, vec3 endView, bool inTexture);
Ray LinearSearch(Ray ray, vec3 startView, vec3 endView, bool inTexture);
Ray RayMarch(vec2 texCoord, bool inTexture);




float getDepth(vec2 coords)       { return texture2D(gPosition, coords).w; }
vec3  getPosition(vec2 coords)    { return texture2D(gPosition, coords).xyz; }
vec3  getNormal(vec2 coords)      { return texture2D(gNormal, coords).xyz; }
vec3  getAlbedo(vec2 coords)      { return texture2D(gAlbedoSpec, coords).xyz; }
float getRoughness(vec2 coords)   { return texture2D(gNormal, coords).w; }
float getMetalicness(vec2 coords) { return texture2D(gAlbedoSpec, coords).w; }

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

vec3 getShading(vec3 fragPos, vec3 fragNormal, vec3 fragAlbedo, float fragRoughness, float fragMetalicness) {
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

vec3 getRendered(vec2 coords) {
	vec3  fragPos         = getPosition(coords);
    vec3  fragNormal      = getNormal(coords);
    vec3  fragAlbedo      = getAlbedo(coords);
    float fragMetalicness = getMetalicness(coords);
    float fragRoughness   = getRoughness(coords);

	return getShading(fragPos, fragNormal, fragAlbedo, fragRoughness, fragMetalicness);
}


float SSR_distance = 9.0f;
uniform int SSR_linear_steps = 100;
int SSR_binary_steps = 10;
uniform float SSR_thickness = 0.1f;
float reflectionSpecularFalloffExponent = 3.0;

Ray BinarySearch(Ray ray, vec3 startView, vec3 endView, bool inTexture) {
	float coeff = 1.0f;
	float viewDistance;
	float textureDistance;
	float depth;
	float steps = ray.steps;

	for (int i=0; i<SSR_binary_steps; i++) {
		coeff *= 0.5f;
		
		// Computes depth
        textureDistance = getPosition(ray.currPosUV).z;
        viewDistance = (startView.z * endView.z) / mix(endView.z, startView.z, steps / float(SSR_linear_steps));
        depth = textureDistance - viewDistance;


		float dir = 1.0f;
		steps += coeff;
		if(depth >= 0.0f) {
			dir = - 1.0f;
			steps -= 2* coeff;
		}

		if (inTexture == true) {
			ray.currPosUV += coeff * dir * ray.incrementUV;
		}
		else {
			ray.currPos += coeff * dir * ray.increment;
			ray.currPosUV = getUV(ray.currPos);
		}
		
		// If UV is out of texture
        if (ray.currPosUV.x > 1. || ray.currPosUV.x < 0 || ray.currPosUV.y > 1. || ray.currPosUV.y < 0) return ray;
	}

	return ray;
}


Ray LinearSearch(Ray ray, vec3 startView, vec3 endView, bool inTexture) {
	// Declare variables in advance
	float textureDistance;
    float viewDistance;
    float depth;
    float diff_normal;

	// Ray march
	while(ray.steps < SSR_linear_steps)
	{
		ray.steps++;
		if (inTexture == true) {
			ray.currPosUV += ray.incrementUV;
		}
		else {
			ray.currPos += ray.increment;
			ray.currPosUV = getUV(ray.currPos);
		}

		// If UV is out of texture
        if (ray.currPosUV.x > 1. || ray.currPosUV.x < 0 || ray.currPosUV.y > 1. || ray.currPosUV.y < 0) return ray;

		// Computes depth
		// This methods is quite similar but more noisy
        // textureDistance = getDepth(ray.currPosUV);
		// viewDistance = length(startView + ray.steps * ray.increment);

		// Computes depth
        textureDistance = getPosition(ray.currPosUV).z;
        viewDistance = (startView.z * endView.z) / mix(endView.z, startView.z, ray.steps/ float(SSR_linear_steps));
        depth = textureDistance - viewDistance;
        diff_normal = distance(ray.viewNormal, getNormal(ray.currPosUV));

        if ( (abs(depth) < SSR_thickness) && (diff_normal > 0.1f)){
			if(useBinary) ray = BinarySearch(ray, startView, endView, inTexture);

            vec3  reflectionPosFrom  = normalize(ray.origin);
            ray.reflected    = normalize(reflect(getNormal(ray.currPosUV), ray.viewNormal));
            vec3  reflectedAlbedo    = getAlbedo(ray.currPosUV);
            float reflectedRoughness = getRoughness(ray.currPosUV);
            float reflectedMetallic  = getMetalicness(ray.currPosUV);
            ray.out_col = getShading (reflectionPosFrom, ray.reflected, reflectedAlbedo, reflectedRoughness, reflectedMetallic);
			ray.hit = true;
            return ray;
        }
    }

	return ray;
}

Ray RayMarch(vec2 texCoord, bool inTexture)
{
	// First let's check that we need to compute a ray march
	Ray ray;
	ray.hit = false;
	float fragMetalicness = getMetalicness(texCoord);
	if (fragMetalicness < 0.05f) return ray;

    // Everything is computed in the view space
	ray.origin = getPosition(texCoord);
	vec3 V = normalize(ray.origin);
	ray.viewNormal = normalize(getNormal(texCoord));
	ray.direction  = normalize(reflect(V, ray.viewNormal));

	// Iniate ray march
	ray.out_col = vec3(0.0);
	ray.steps = 0;

	// Compute ray step in view space
    vec3 startView = ray.origin;
    vec3 endView   = ray.origin + (ray.direction * SSR_distance * extent);
    if (!(allowBehindCamera) && (endView.z > 0.0)) endView = ray.origin + (0.0 - ray.origin.z) / ray.direction.z * ray.direction;
	ray.increment  = (endView - startView) / float(SSR_linear_steps);
	ray.currPos = startView;

	// Compute ray step in texture
    vec2 startFragUV = getUV(startView);
    vec2 endFragUV   = getUV(endView);
	ray.incrementUV = (endFragUV - startFragUV) / float(SSR_linear_steps);
	ray.currPosUV = startFragUV;

    // Ray march
	return LinearSearch(ray, startView, endView, inTexture);
}

// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}



// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }



// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}



// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }

vec3 SSR(vec2 texCoord, vec3 fragRendered) {
	float SSR_multiplier = 0.0f;
	vec3 SSR_color = vec3(0);

	int alias = 1;
	if (useAntiAlias) alias = 8;
	int nb_hits = 0;
	float rand = random(texCoord);
	Ray ray;

	vec3 outCol = vec3(0);
	vec2 uv = vec2(0);
	for(int i=0; i<alias; i++) {
		float dx = random(i) * rand;
		float dy = random(i+0.5f) * rand;
		dx *= 0.003f;
		dy *= 0.003f;
    	ray = RayMarch(texCoord + vec2(dx, dy), useInTexture);
		if (ray.hit == true) {
			nb_hits++;
			outCol += ray.out_col;
			uv += ray.currPosUV;
		}
	}

    float fragMetalicness = getMetalicness(texCoord);
    vec3 fragNormal = getNormal(texCoord);
    vec3 fragAlbedo = getAlbedo(texCoord);

	bool hit = false;
	if (nb_hits > max(0, 0.5f * alias)) hit = true;
	if (useDirectionShading && (ray.reflected.z <= 0)) hit = false;

	if (hit) {
		uv /= float(nb_hits);
		outCol /= float(nb_hits);
		vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - uv));
		float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0f, 1.0f);
		float multiplier = pow(fragMetalicness, reflectionSpecularFalloffExponent);
		if (useScreenEdge) multiplier *= screenEdgefactor;
		if (useDirectionShading) multiplier *= ray.reflected.z;
		SSR_multiplier = clamp(multiplier, 0.0f, 0.9f);
		SSR_color = outCol;
	}

	vec3 r = fragRendered + (SSR_color - fragRendered) * SSR_multiplier;
	
	if(diagnostic == 5)
		return ray.direction;
	else if(!(useReflectedShading)) {
		if (hit) return outCol;
        else return r;
    }
	else
		return r;
}




void main () {
	vec3 RENDER     = getRendered(TexCoords);
	vec3 RENDER_SSR = SSR(TexCoords, RENDER);

	float fragMetalicness = getMetalicness(TexCoords);
    vec3 fragNormal = getNormal(TexCoords);
    vec3 fragAlbedo = getAlbedo(TexCoords);
	
    
	if(diagnostic == 2)
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
	else
		FragColor = vec4 (RENDER_SSR, 1.0);
}
