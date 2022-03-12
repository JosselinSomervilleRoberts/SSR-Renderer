#version 450 core

uniform sampler2D gAlbedoSpec;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
in vec2 TexCoords;

out vec4 FragColor;


uniform mat4 projectionMat;




void main()
{
    vec3 viewNormal = texture2D(gNormal, TexCoords).xyz;
    vec3 viewPos = texture2D(gPosition, TexCoords).xyz;


    // Reflection vector
    vec3 reflected = normalize(reflect(normalize(viewPos), normalize(viewNormal)));

    FragColor = vec4(reflected, 1.0f);
    vec4 pos = texture2D(gPosition, TexCoords);//inverse(projectionMat) * vec4(texture2D(gPosition, TexCoords).xyz, 1.0);
    FragColor = vec4(0.004f * (pos.w - 400) , 0, 0, 1);
}