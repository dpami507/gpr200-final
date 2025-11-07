#version 330 core

out vec4 FragColor;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in vec3 PerlinNoise;

uniform int shadingMode;

// texture samplers
uniform sampler2D texture1;

//Material Controls
struct Material {
    float ambient;
    float diffuse;
    float specular;
    float shininess;
}; 
uniform Material material;

//Light Object
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightStrength;
uniform float lightFalloff;

uniform vec3 viewPos;
uniform float uTime;

void main()
{
	//Color
	float pNoiseLength = length(PerlinNoise);
	vec3 color = vec3(1, 0, 0); 
	if(pNoiseLength <= 1) color = vec3(1, 1, 1);			//Snow
	if(pNoiseLength <= 0.7) color = vec3(0.5, 0.5, 0.5);	//Stone
	if(pNoiseLength <= 0.4) color = vec3(0.5, 1, 0.5);		//Grass
	if(pNoiseLength <= 0.15) color = vec3(1, 1, 0.7);		//Sand
	if(pNoiseLength <= 0.1) color = vec3(0, 0, 1);			//Water

	//Set ambient Light
    vec3 ambient = material.ambient * lightColor;

	//Distance
	float dist = min(distance(lightPos, FragPos), lightFalloff);
	float power = max(-lightStrength * (pow(dist / lightFalloff, 2) - 1), 0); //https://www.desmos.com/calculator/qyhevth3pc This was very weird to make :/

	//Lighting
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);  
	vec3 viewDir    = normalize(viewPos - FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	//Diffuse
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor * material.diffuse;

	//Specular BlinnPhon
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	vec3 specular = lightColor * spec * material.specular;

	//Phong
    vec3 phongResult = (ambient + (diffuse + specular) * power) * color;

	//UVs
	//Normals
	//Shaded
//	if(shadingMode == 0)
//		FragColor = vec4(TexCoord, 0.0, 1.0);
//    else if(shadingMode == 1)
//		FragColor = vec4(abs(Normal), 1.0);
//	else if(shadingMode == 2)
//		FragColor = vec4(phongResult, 1.0);
//	else
//		FragColor = vec4(1.0, 0.0, 0.0, 1.0);

	FragColor = vec4(phongResult, 1.0);
}