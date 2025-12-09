#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 FragPos; 
out vec3 Normal;

uniform mat4 model;
uniform mat4 projectionView;
uniform float uTime;

uniform sampler2D cellularNoise;

uniform float width;
uniform float height;
uniform float speed;

float getWaveCalc(vec3 pos)
{
	float waveCalc = 0;

	//Fract it for more waves
	vec2 uv = aTexCoord;
	uv *= 4.0;
    uv = fract(uv);

	//Move the waves
    uv.x += uTime * speed * 2.0;
    uv.y -= uTime * speed * 2.0;

	//Move the vertex up based on the height
	waveCalc = texture(cellularNoise, uv).r * height;
	waveCalc += sin(width * ((pos.x + pos.z) + (uTime * speed))) * height;

	return waveCalc;
}

void main()
{
	vec3 pos = aPos;
	float waveCalc = getWaveCalc(pos);

	pos.y = waveCalc;

	//Set up out variables
	gl_Position = projectionView * model * vec4(pos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	FragPos = vec3(model * vec4(pos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;  
}