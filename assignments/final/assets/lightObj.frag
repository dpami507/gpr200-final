#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform float lightStrength;
uniform vec3 lightColor;

void main()
{
    vec3 result = lightStrength * lightColor;
    FragColor = vec4(result, 1.0);
}