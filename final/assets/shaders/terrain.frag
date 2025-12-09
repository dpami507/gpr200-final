#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

//Noise
uniform float heightScale;

//Materials
struct Material {
    sampler2D colorTex;
    sampler2D aoTex;
    sampler2D normalTex;
    sampler2D roughTex;
    float threshold;
    float uvTile;
};

uniform int numMaterials;
const int MAX_TERRAIN_MATERIALS = 5;
uniform Material materials[MAX_TERRAIN_MATERIALS];

//Noise Prototypes
float perlin(float x, float y);
float octavePerlin(float x, float y);
float grad(int hash, float x, float y);
float fade(float t);
float lerp(float a, float b, float t);

//This is used to check if drawing has occured and update colors based on it
uniform sampler2D noiseTexture;

//Skybox
uniform samplerCube skybox;
uniform vec3 viewPos;

//Light
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform float lightIntensity;

uniform int shadingMode = 0;

//Get Color with all the maps and light inculded
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
//get the normal from the normal map provided
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
    //Calculate values for skybox reflections
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-viewDir, Normal);
    vec3 skyboxReflection = texture(skybox, reflectDir).rgb;

    //Default values
	vec2 pos = FragPos.xz;
    vec3 color = vec3(1.0, 0.0, 0.0);
    vec3 finalColor = vec3(1.0, 0.0, 0.0);
    float ao = 1.0;
    float rough = 0.5;
    vec3 norm = vec3(0.5, 0.5, 1.0);

    float height = texture(noiseTexture, TexCoord).r;

    height = height * 2.0 - 1.0; //Convert from 0-1 to -1 to 1
	height *= heightScale; //Height scale
	height = (height + 1.0) * 0.5; //Normalize to 0-1

    vec2 uv = TexCoord;

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
        //Fract it a bit
        uv.xy *= materials[0].uvTile;
        uv = fract(uv);

        //Only one texture, use it for everything
        color = texture(materials[0].colorTex, uv).rgb;
        ao = texture(materials[0].aoTex, uv).r;
        rough = texture(materials[0].roughTex, uv).r;
        norm = getNormalFromMap(materials[0].normalTex);
    }
    else if (numMaterials > 1)
    {
        //Get slected material
        int selectedMaterial = 0;
        for (int i = 0; i < numMaterials; i++)
        {
            if (height > materials[i].threshold)
            {
                selectedMaterial = i;
            }
        }

        //Mix material is the material above it
        int mixedMaterial = clamp(selectedMaterial + 1, 0, numMaterials - 1);

        //UV fracting
        uv.xy *= materials[selectedMaterial].uvTile;
        uv = fract(uv);

        //The amount of height that it blends over
        float blendAmount = 0.05;

        //The min blendAmount is the threshold of the selected material
        float minBlendAmount = materials[selectedMaterial].threshold;

        //Calculate mix amount
        float mixAmount = (height - minBlendAmount) / blendAmount;
        mixAmount = clamp(mixAmount, 0.0, 1.0);

        //Mix textures
        color = mix(texture(materials[selectedMaterial].colorTex, uv).rgb, texture(materials[mixedMaterial].colorTex, uv).rgb, mixAmount);
        ao = mix(texture(materials[selectedMaterial].aoTex, uv).r, texture(materials[mixedMaterial].aoTex, uv).r, mixAmount);
        rough = mix(texture(materials[selectedMaterial].roughTex, uv).r, texture(materials[mixedMaterial].roughTex, uv).r, mixAmount);
        norm = mix(getNormalFromMap(materials[selectedMaterial].normalTex),  getNormalFromMap(materials[mixedMaterial].normalTex), mixAmount);
    }
    
    finalColor = computeDirectionalLight(norm, viewDir, color, rough, ao);

    //Calculate how strong to reflect skybox based on the roughness texture
    float reflectionStrength = 1.0 - rough;

    //Shaded, UV, Normals
    switch(shadingMode % 3)
    {
        case 0:
            finalColor = mix(finalColor, skyboxReflection, reflectionStrength * 0.05);
            break;
        case 1:
            finalColor = vec3(abs(uv), 0.0);
            break;
        case 2:
            finalColor = abs(norm);
            break;
    }

    FragColor = vec4(finalColor, 1.0);
}