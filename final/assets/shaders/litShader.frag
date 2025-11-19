#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform int shadingMode;

// texture samplers
uniform sampler2D texture1;
uniform vec2 uvTiling;
uniform vec3 color;

//Material Controls
struct Material {
    float ambient;
    float diffuse;
    float specular;
    float shininess;
}; 
uniform Material material;

//Light
struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;

uniform vec3 viewPos;

uniform samplerCube skybox;

void main()
{
    vec2 uv = TexCoord;
    uv.x *= uvTiling.x;
    uv.y *= uvTiling.y;
    uv = fract(uv);

	//Texture
	vec3 tex = texture(texture1, uv).rgb;

	//Lighting
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	vec3 viewDir    = normalize(viewPos - FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	//Set ambient Light
	vec3 ambient = light.ambient * material.ambient;

	//Diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	//Specular BlinnPhon
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec;

	//Phong
    vec3 phongResult = (ambient + (diffuse + specular)) * (tex * (color / 255));

	//UVs
	//Normals
	//Shaded
	if(shadingMode == 0)
		FragColor = vec4(uv, 0.0, 1.0);
    else if(shadingMode == 1)
		FragColor = vec4(abs(Normal), 1.0);
	else if(shadingMode == 2)
		FragColor = vec4(phongResult, 1.0);
	else
		FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}