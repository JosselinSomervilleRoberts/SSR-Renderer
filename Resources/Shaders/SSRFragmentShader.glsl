#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;


void main(){
    FragColor = texture( gAlbedoSpec, TexCoords);// + 0.005*vec2( sin(time+1024.0*fTexCoord.x),cos(time+768.0*fTexCoord.y)) ).xyz;//vec3(1,0,0);//texture(renderedTexture, fTexCoord);//vec3(1,0,0);//texture( renderedTexture, UV + 0.005*vec2( sin(time+1024.0*UV.x),cos(time+768.0*UV.y)) ).xyz;
}
