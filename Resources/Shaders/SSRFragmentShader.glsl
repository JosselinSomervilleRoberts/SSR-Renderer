#version 330 core

in vec2 fTexCoord;

out vec3 color;

uniform sampler2D renderedTexture;
uniform float time;

void main(){
    color = texture( renderedTexture, fTexCoord + 0.005*vec2( sin(time+1024.0*fTexCoord.x),cos(time+768.0*fTexCoord.y)) ).xyz;//vec3(1,0,0);//texture(renderedTexture, fTexCoord);//vec3(1,0,0);//texture( renderedTexture, UV + 0.005*vec2( sin(time+1024.0*UV.x),cos(time+768.0*UV.y)) ).xyz;
}