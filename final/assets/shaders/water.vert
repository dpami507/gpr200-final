#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 FragPos; 
out vec3 Normal;

out float waveCalc;

uniform mat4 model;
uniform mat4 projectionView;
uniform float uTime;

uniform float w;
uniform float h;
uniform float s;

void main()
{
	//Create Waves
	vec3 pos = aPos;
	pos.y += sin(w * ((pos.x + pos.z) + (uTime * s))) * h;
	waveCalc = pos.y / h;

	//Set up out variables
	gl_Position = projectionView * model * vec4(pos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	FragPos = vec3(model * vec4(pos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;  
}