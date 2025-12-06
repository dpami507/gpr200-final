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

uniform sampler2D cellularNoise;

uniform float width;
uniform float height;
uniform float speed;

void main()
{
	//Create Waves
	vec3 pos = aPos;

	//Fract it for more waves
	vec2 uv = aTexCoord;
	uv *= 4.0;
    uv = fract(uv);

	//Move the waves
    uv.x += uTime * speed * 2.0;
    uv.y -= uTime * speed * 2.0;

	//Move the vertex up based on the height
	pos.y = texture(cellularNoise, uv).r * height;
	pos.y += sin(width * ((pos.x + pos.z) + (uTime * speed))) * height;
	waveCalc = pos.y / height;

	//Set up out variables
	gl_Position = projectionView * model * vec4(pos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	FragPos = vec3(model * vec4(pos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;  
}