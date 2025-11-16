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

//Light Object
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightStrength;
uniform float lightFalloff;

uniform vec3 viewPos;

uniform samplerCube skybox;

void main()
{
    vec2 uv = TexCoord;
    uv.x *= uvTiling.x;
    uv.y *= uvTiling.y;
    uv = fract(uv);

	//Texture
	vec4 tex = texture(texture1, uv);

	//Lighting
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);  
	vec3 viewDir    = normalize(viewPos - FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	//Set ambient Light
	vec3 skyAmbient = texture(skybox, normal).rgb;
	vec3 ambient = material.ambient * mix(lightColor, skyAmbient, 0.7);

	//Distance
	float dist = min(distance(lightPos, FragPos), lightFalloff);
	float power = max(-lightStrength * (pow(dist / lightFalloff, 2) - 1), 0); //https://www.desmos.com/calculator/qyhevth3pc This was very weird to make :/

	//Diffuse
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor * material.diffuse;

	//Specular BlinnPhon
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	vec3 specular = lightColor * spec * material.specular;

	//Skybox reflections
	vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, normal);
	vec3 reflection = texture(skybox, -R).rgb;

	float fresnel = pow(1.0 - max(dot(normal, -I), 0.0), 3.0);
	float reflectivity = material.specular * fresnel;

	//Phong
    vec3 phongResult = (ambient + (diffuse + specular) * power) * (vec3(tex) * (color / 255));

	//UVs
	//Normals
	//Shaded
	if(shadingMode == 0)
		FragColor = vec4(uv, 0.0, 1.0);
    else if(shadingMode == 1)
		FragColor = vec4(abs(Normal), 1.0);
	else if(shadingMode == 2)
	{
		vec3 finalResult = mix(phongResult, reflection, reflectivity);
		FragColor = vec4(finalResult, 1.0);
	}
	else
		FragColor = vec4(1.0, 0.0, 0.0, 1.0);

	//FragColor = vec4(texture(skybox, vec3(0, 1, 0)).rgb, 1.0);
}