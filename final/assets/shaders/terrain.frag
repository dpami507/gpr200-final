#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform float frequency;
uniform int octaves;
uniform float heightScale;

struct Material {
    sampler2D colorTex;
    sampler2D aoTex;
    sampler2D normalTex;
    sampler2D roughTex;
    float threshold;
    float uvTile;
};

const int MAX_TERRAIN_MATERIALS = 5;
uniform Material materials[MAX_TERRAIN_MATERIALS];
uniform int numMaterials;

float perlin(float x, float y);
float octavePerlin(float x, float y);
float grad(int hash, float x, float y);
float fade(float t);
float lerp(float a, float b, float t);

//This is used to check if drawing has occured and update colors based on it
uniform sampler2D noiseTexture;

uniform samplerCube skybox;
uniform vec3 viewPos;

uniform vec3 lightDirection;  // normalized
uniform vec3 lightColor;
uniform float lightIntensity;

int permutation[256] = int[256] ( 151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
);

int p(int i) {
    return permutation[i & 255];
}

vec3 computeDirectionalLight(vec3 normal, vec3 viewDir, vec3 albedo, float rough, float ao)
{
    vec3 L = normalize(-lightDirection);
    vec3 N = normalize(normal);

    // Diffuse
    float diff = max(dot(N, L), 0.0);

    // Specular (simple Blinn-Phong)
    vec3 H = normalize(L + viewDir);
    float shininess = mix(256.0, 2.0, rough); 
    float spec = pow(max(dot(N, H), 0.0), shininess);
    float specularStrength = clamp(1.0 - rough * 2.0, 0.0, 1.0);
    spec *= specularStrength;

    vec3 color =
        albedo * diff * lightColor * lightIntensity + 
        spec * lightColor * lightIntensity;           

    return color * ao;
}

vec3 getNormalFromMap(sampler2D normMap)
{
    vec3 tangentNormal = texture(normMap, TexCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoord);
    vec2 st2 = dFdy(TexCoord);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-viewDir, Normal);
    vec3 skyboxReflection = texture(skybox, reflectDir).rgb;

	vec2 pos = FragPos.xz;
    vec3 color = vec3(1.0, 0.0, 0.0);
    vec3 finalColor = vec3(1.0, 0.0, 0.0);
    float ao = 1.0;
    float rough = 0.5;
    vec3 norm = vec3(1.0, 0.0, 0.0);

    float height = octavePerlin(pos.x, pos.y);
    float differenceInHeight = height - texture(noiseTexture, TexCoord).r;
    height -= differenceInHeight;

    height = height * 2.0 - 1.0; //Convert from 0-1 to -1 to 1
	height *= heightScale; //Height scale
	height = (height + 1.0) * 0.5; //Normalize to 0-1

    //Get texture sample based on height
    if (numMaterials == 0)
    {
        //Fallback green color
        color = vec3(0.0, 1.0, 0.0);
        ao = 1.0;
        rough = 0.5;
        norm = vec3(0.5, 0.5, 1.0);
    }
    else if(numMaterials == 1)
    {
        vec2 uv = TexCoord;
        uv.xy *= materials[0].uvTile;
        uv = fract(uv);

        //Only one texture, use it for everything
        color = texture(materials[0].colorTex, uv).rgb;
        ao = texture(materials[0].aoTex, uv).r;
        rough = texture(materials[0].roughTex, uv).r;
        norm = getNormalFromMap(materials[0].normalTex);

        finalColor = computeDirectionalLight(norm, viewDir, color, rough, ao);
    }
    else
    {
        int selectedMaterial = 0;
        for (int i = 0; i < numMaterials; i++)
        {
            if (height > materials[i].threshold)
            {
                selectedMaterial = i;
            }
        }

        vec2 uv = TexCoord;
        uv.xy *= materials[selectedMaterial].uvTile;
        uv = fract(uv);

        color = texture(materials[selectedMaterial].colorTex, uv).rgb;
        ao = texture(materials[selectedMaterial].aoTex, uv).r;
        rough = texture(materials[selectedMaterial].roughTex, uv).r;
        norm = getNormalFromMap(materials[selectedMaterial].normalTex);

        finalColor = computeDirectionalLight(norm, viewDir, color, rough, ao);
    }
    
    float reflectionStrength = 1.0 - rough;
    finalColor = mix(finalColor, skyboxReflection, reflectionStrength * 0.05);
    FragColor = vec4(finalColor, 1.0);
}

float perlin(float x, float y)
{
    int xi = int(floor(x)) & 255;
    int yi = int(floor(y)) & 255;
    
    float xf = x - floor(x);
    float yf = y - floor(y);
    
    float u = fade(xf);
    float v = fade(yf);
    
    int aa = p(p(xi) + yi);
    int ab = p(p(xi) + yi + 1);
    int ba = p(p(xi + 1) + yi);
    int bb = p(p(xi + 1) + yi + 1);
    
    float d1 = grad(aa, xf, yf);
    float d2 = grad(ba, xf - 1.0, yf);
    float d3 = grad(ab, xf, yf - 1.0);
    float d4 = grad(bb, xf - 1.0, yf - 1.0);
    
    float x1 = lerp(d1, d2, u);
    float x2 = lerp(d3, d4, u);
    
    float yInter = lerp(x1, x2, v);
    
    return (yInter + 1.0) * 0.5;
}

float octavePerlin(float x, float y)
{
	float total = 0;
    float persistence = 0.5;
	float freq = frequency;
	float amp = 1;
	float maxValue = 0;
	for (int i = 0; i < octaves; i++)
	{
		total += perlin(x * freq, y * freq) * amp;

		maxValue += amp;

		amp *= persistence;
		freq *= 2.0f;
	}

	float noiseValue = total / maxValue;

	return noiseValue;
}

float grad(int hash, float x, float y)
{
	switch (hash & 3)
	{
	case 0: return  x + y;
	case 1: return  -x + y;
	case 2: return  x - y;
	case 3: return  -x - y;
	default: return 0;
	}
}

float fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float a, float b, float t) 
{
    return a + t * (b - a);
}