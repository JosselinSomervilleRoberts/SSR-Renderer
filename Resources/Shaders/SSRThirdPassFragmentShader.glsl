#version 450 core
#define PI 3.14159

uniform int diagnostic = 1;
uniform sampler2D gSSR;
in vec2 TexCoords;

out vec4 FragColor;

void main () {
	FragColor = vec4(texture(gSSR, TexCoords).xyz, 1.0f);
}
