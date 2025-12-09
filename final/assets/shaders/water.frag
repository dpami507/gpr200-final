#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform samplerCube skybox;
uniform vec3 viewPos;

uniform sampler2D cellularNoise;
uniform float width;
uniform float speed;
uniform float height;
uniform float uTime;

float getWaveCalc(vec3 pos)
{
	float waveCalc = 0;

	//Fract it for more waves
	vec2 uv = TexCoord;
	uv *= 4.0;
    uv = fract(uv);

	//Move the waves
    uv.x += uTime * speed * 2.0;
    uv.y -= uTime * speed * 2.0;

	//Move the vertex up based on the height
	waveCalc = texture(cellularNoise, uv).r * height;
	waveCalc += sin(width * ((pos.x + pos.z) + (uTime * speed)));

	return waveCalc;
}

vec3 getNormal(vec2 uv)
{
    //Get sample offset
    float sampleOffset = 0.01;
    vec3 pos = FragPos;

    //Get vertical tangent vector
    float topHeight = getWaveCalc(vec3(pos.x, 0, pos.z + sampleOffset)); 
    topHeight += texture(cellularNoise, vec2(uv.x, uv.y + sampleOffset)).r / 10;
    topHeight *= height;
    float bottomHeight = getWaveCalc(vec3(pos.x, 0, pos.z - sampleOffset));
    bottomHeight += texture(cellularNoise, vec2(uv.x, uv.y - sampleOffset)).r / 10;
    bottomHeight *= height;
    float rightHeight = getWaveCalc(vec3(pos.x + sampleOffset, 0, pos.z));
    rightHeight += texture(cellularNoise, vec2(uv.x + sampleOffset, uv.y)).r / 10;
    rightHeight *= height;
    float leftHeight = getWaveCalc(vec3(pos.x - sampleOffset, 0, pos.z));
    leftHeight += texture(cellularNoise, vec2(uv.x - sampleOffset, uv.y)).r / 10;
    leftHeight *= height;

    vec3 vertical = vec3(0.0, topHeight - bottomHeight, sampleOffset * 2.0);
    vec3 horizontal = vec3(sampleOffset * 2.0, rightHeight - leftHeight, 0.0);

    //Get the normal from the points
	vec3 normal = normalize(cross(horizontal, vertical));

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