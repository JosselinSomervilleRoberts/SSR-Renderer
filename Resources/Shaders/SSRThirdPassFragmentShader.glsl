#version 450 core
#define PI 3.14159

uniform int diagnostic = 1;
//uniform sampler2D gRendered;
in vec2 TexCoords;

out vec4 FragColor;

void main () {
    FragColor = vec4(1,0,1,1);
	//FragColor = vec4(texture(gRendered, TexCoords).xyz, 1.0f);
}
