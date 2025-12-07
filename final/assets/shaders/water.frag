#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

in float waveCalc;

uniform samplerCube skybox;
uniform vec3 viewPos;

uniform sampler2D cellularNoise;
uniform float width;
uniform float speed;
uniform float height;
uniform float uTime;

vec3 getNormal(vec2 uv)
{
    //Get sample offset
    float sampleOffset = 0.01;
    vec3 pos = FragPos;

    //Get heights of three points
    float topHeight         = texture(cellularNoise, uv + vec2(0.0,            sampleOffset)).r * (height / 10);
    topHeight += sin(width * ((pos.x + pos.z + sampleOffset) + (uTime * speed))) * height;

	float bottomLeftHeight  = texture(cellularNoise, uv + vec2(-sampleOffset, -sampleOffset)).r * (height / 10);
    bottomLeftHeight += sin(width * ((pos.x + pos.z - sampleOffset) + (uTime * speed))) * height;

	float bottomRightHeight = texture(cellularNoise, uv + vec2( sampleOffset, -sampleOffset)).r * (height / 10);
    bottomRightHeight += sin(width * ((pos.x + pos.z) + (uTime * speed))) * height;

    //Define the three points
	vec3 A = vec3(uv.x, topHeight, uv.y + sampleOffset);						// 	  A
	vec3 B = vec3(uv.x - sampleOffset, bottomLeftHeight, uv.y - sampleOffset);	//	 /.\ 
	vec3 C = vec3(uv.x + sampleOffset, bottomRightHeight, uv.y - sampleOffset);	//	B---C

    //Get the normal from the points
	vec3 normal = normalize(cross((B - A), (C - A)));

    return normal;
}

void main()
{
    //Fract it for more waves
    vec2 uv = TexCoord;
    uv *= 4.0;
    uv = fract(uv);

    //Move the waves
    uv.x += uTime * speed * 2.0;
    uv.y -= uTime * speed * 2.0;

    //Get the normal vector
    vec3 normal = getNormal(uv);

    //Get the reflection
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-viewDir, normal);
    vec3 skyboxReflection = texture(skybox, reflectDir).rgb;

    //Set the color
    vec3 waterColor = mix(vec3(0.2, 0.3, 0.5), skyboxReflection, 0.5);

    FragColor = vec4(waterColor, 0.5);
}